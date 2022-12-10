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
#include "imgui/misc/freetype/imgui_freetype.h"
#include "fonts.h"

namespace Turbine
{

std::array<ImFont*, static_cast<size_t>(FontId::Count)> Fonts::m_Fonts;

void Fonts::Load()
{
	AddFont(FontId::DroidSans16, "fonts/droid-sans.regular.ttf", 16.0f);
	AddFont(FontId::AltoVoltaje32, "fonts/altovoltaje/Alto Voltaje.ttf", 32.0f, 2.0f);
	AddFont(FontId::Inconsolata18, "fonts/inconsolata/Inconsolata.otf", 18.0f);
}

ImFont* Fonts::GetFont(FontId id)
{
	SDL_assert(id != FontId::Count);
	return m_Fonts[static_cast<size_t>(id)];
}

void Fonts::AddFont(FontId id, const char* pPath, float size, float glyphExtraSpacing /* = 0.0f */)
{
	SDL_assert(id != FontId::Count);
	SDL_assert(size >= 8.0f);

	ImFontConfig fontConfig;
	fontConfig.GlyphExtraSpacing = ImVec2(glyphExtraSpacing, 0.0f);

	m_Fonts[static_cast<size_t>(id)] = ImGui::GetIO().Fonts->AddFontFromFileTTF(pPath, size, &fontConfig);
}

} // namespace Turbine