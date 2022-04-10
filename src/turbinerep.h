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

#include <array>
#include <memory>
#include <string>

#include <SDL_opengl.h>

#include "camera.h"
#include "camerarep.h"

struct SDL_Surface;
struct SDL_Window;

namespace Turbine
{

class Atlas;
using AtlasUniquePtr = std::unique_ptr<Atlas>;
class MainMenuBar;
using MainMenuBarUniquePtr = std::unique_ptr<MainMenuBar>;

class TurbineRep
{
public:
	TurbineRep(SDL_Window* pWindow);
	~TurbineRep();

	void ProcessEvent(const SDL_Event& event);
	void Update(float delta);
	void Render();

private:
	void SetUserInterfaceStyle();
	CameraVector GetHoveredCameras();

	void RenderCameras();
	void OpenPickedCamera();
	void FlushClosedCameras();
	const ImColor& GetPinColor(Camera::State state) const;

	SDL_Window* m_pWindow;
	AtlasUniquePtr m_pAtlas;
	float m_CellSize;
    MainMenuBarUniquePtr m_pMainMenuBar;
	
	using CameraRepList = std::list<CameraRep>;
	CameraRepList m_CameraReps;
	bool m_SelectCamera;

	std::array<ImColor, static_cast<size_t>(Camera::State::Count)> m_PinColor;
};

} // namespace Turbine
