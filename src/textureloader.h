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

#pragma once

#include <SDL_opengl.h>

#include <filesystem>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace Turbine
{

class TextureLoader
{
public:
	static void Initialise();
	static void Update();
	static GLuint LoadTexture(const std::filesystem::path& path);
	static void UnloadTexture(GLuint texture);

private:
	static void ProcessQueuedLoads();
	static void ProcessQueuedUnloads();

	static std::thread::id m_MainThreadId;
	static std::queue<std::filesystem::path> m_TextureLoadQueue;
	struct TextureLoadResult
	{
		std::filesystem::path path;
		GLuint texture;
	};
	static std::queue<TextureLoadResult> m_TextureLoadResultQueue;
	static std::mutex m_LoadMutex;
	static std::mutex m_ResultMutex;
	static std::queue<GLuint> m_TextureUnloadQueue;
	static std::mutex m_UnloadMutex;
};

} // namespace Watcher
