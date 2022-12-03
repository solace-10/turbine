/*
MIT License

Copyright (c) 2022 Pedro Nunes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <SDL.h>
#include <SDL_image.h>

#include "log.h"
#include "textureloader.h"
#include "encoding.h"

namespace Turbine
{

std::thread::id TextureLoader::m_MainThreadId;
std::queue<std::filesystem::path> TextureLoader::m_TextureLoadQueue;
std::queue<TextureLoader::TextureLoadResult> TextureLoader::m_TextureLoadResultQueue;
std::mutex TextureLoader::m_LoadMutex;
std::mutex TextureLoader::m_ResultMutex;
std::queue<GLuint> TextureLoader::m_TextureUnloadQueue;
std::mutex TextureLoader::m_UnloadMutex;

void TextureLoader::Initialise()
{
	m_MainThreadId = std::this_thread::get_id();
}

void TextureLoader::Update()
{
	ProcessQueuedLoads();
	ProcessQueuedUnloads();
}

void TextureLoader::ProcessQueuedLoads()
{
	std::lock_guard< std::mutex > loadLock(m_LoadMutex);
	while (m_TextureLoadQueue.empty() == false)
	{
		GLuint texture = LoadTexture(m_TextureLoadQueue.front());
		{
			std::lock_guard< std::mutex > resultLock(m_ResultMutex);
			TextureLoadResult result;
			result.path = m_TextureLoadQueue.front();
			result.texture = texture;
			m_TextureLoadResultQueue.push(result);
			m_TextureLoadQueue.pop();
		}
	}
}

void TextureLoader::ProcessQueuedUnloads()
{
	std::lock_guard<std::mutex> unloadLock(m_UnloadMutex);
	while (m_TextureUnloadQueue.empty() == false)
	{
		GLuint texture = m_TextureUnloadQueue.front();
		glDeleteTextures(1, &texture);
		m_TextureUnloadQueue.pop();
	}
}

GLuint TextureLoader::LoadTexture(const std::filesystem::path& path)
{
	if (std::this_thread::get_id() != m_MainThreadId)
	{
		{
			std::lock_guard< std::mutex > lock(m_LoadMutex);
			m_TextureLoadQueue.push(path);
		}

		while (1)
		{
			std::lock_guard< std::mutex > lock(m_ResultMutex);
			if (m_TextureLoadResultQueue.empty() == false && m_TextureLoadResultQueue.front().path == path)
			{
				GLuint texture = m_TextureLoadResultQueue.front().texture;
				m_TextureLoadResultQueue.pop();
				return texture;
			}
		}
	}

#if _UNICODE
	// Annoyingly, IMG_Load() doesn't actually support wide strings for loading...
	SDL_Surface* pSurface = IMG_Load(Encoding::UTF8Encode(path.c_str()).c_str());
#else
	SDL_Surface* pSurface = IMG_Load(path.c_str());
#endif

	SDL_assert(pSurface != nullptr);
	if (pSurface == nullptr)
	{
		Log::Error("TurbineRep::LoadTexture error: %s", IMG_GetError());
		return 0;
	}

	GLuint tex;
	glGenTextures(1, &tex);
	GLenum err = glGetError();
	glBindTexture(GL_TEXTURE_2D, tex);
	int bpp = pSurface->format->BytesPerPixel;
	if (bpp == 3 || bpp == 4)
	{
		int internalFormat = (bpp == 4) ? GL_RGBA : GL_RGB;
		int format = (bpp == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, pSurface->w, pSurface->h, 0, format, GL_UNSIGNED_BYTE, pSurface->pixels);
	}
	else if (bpp == 1)
	{
		GLubyte* pTexture = new GLubyte[pSurface->w * pSurface->h * 3];
		for (int i = 0; i < pSurface->w * pSurface->h; ++i)
		{
			int j = i * 3;
			GLubyte b = ((GLubyte*)pSurface->pixels)[i];
			pTexture[j] = b;
			pTexture[j + 1] = b;
			pTexture[j + 2] = b;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pSurface->w, pSurface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, pTexture);
		delete[] pTexture;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(pSurface);

	return tex;
}

void TextureLoader::UnloadTexture(GLuint texture)
{
	std::lock_guard< std::mutex > lock(m_UnloadMutex);
	if (std::this_thread::get_id() != m_MainThreadId)
	{
		m_TextureUnloadQueue.push(texture);
	}
	else
	{
		glDeleteTextures(1, &texture);
	}
}

} // namespace Turbine
