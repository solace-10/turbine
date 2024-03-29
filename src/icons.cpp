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
#include "icons.h"
#include "textureloader.h"

namespace Turbine
{

std::array<GLuint, static_cast<size_t>(IconId::Count)> Icons::m_Icons;

void Icons::Load()
{
	m_Icons[static_cast<size_t>(IconId::Issue)] = TextureLoader::LoadTexture("textures/issue.png");
	m_Icons[static_cast<size_t>(IconId::Server)] = TextureLoader::LoadTexture("textures/server.png");
	m_Icons[static_cast<size_t>(IconId::ServerSmall)] = TextureLoader::LoadTexture("textures/server_small.png");
}

GLuint Icons::GetIcon(IconId id)
{
	SDL_assert(id != IconId::Count);
	return m_Icons[static_cast<size_t>(id)];
}

} // namespace Turbine