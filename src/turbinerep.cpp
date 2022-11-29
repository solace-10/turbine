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
#include "imgui/imgui.h"

#include "atlas/atlas.h"
#include "bridge/bridge.h"
#include "bridge/bridgegeolocation.hpp"
#include "icons.h"
#include "log.h"
#include "mainmenubar.hpp"
#include "textureloader.h"
#include "turbinerep.h"
#include "turbine.h"

namespace Turbine
{

static unsigned int sPinWidth = 16;
static unsigned int sPinHeight = 26;
static unsigned int sPinHalfWidth = sPinWidth / 2;

TurbineRep::TurbineRep(SDL_Window* pWindow) :
m_pWindow(pWindow),
m_CellSize(128.0f),
m_SelectCamera(false)
{
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(m_pWindow, &windowWidth, &windowHeight);
	m_pAtlas = std::make_unique<Atlas>(windowWidth, windowHeight);
    m_pMainMenuBar = std::make_unique<MainMenuBar>();

	SetUserInterfaceStyle();

	Icons::Load();

	m_PinColor[static_cast<size_t>(Camera::State::Unknown)]			= ImColor(123, 123, 123);
	m_PinColor[static_cast<size_t>(Camera::State::StreamAvailable)] = ImColor(  0, 200, 0);
	m_PinColor[static_cast<size_t>(Camera::State::Unauthorised)]	= ImColor(255,   0, 0);
}

TurbineRep::~TurbineRep()
{

}

void TurbineRep::SetUserInterfaceStyle()
{
	ImGuiStyle &st = ImGui::GetStyle();
	st.FrameBorderSize = 1.0f;
	st.FramePadding = ImVec2(4.0f, 2.0f);
	st.ItemSpacing = ImVec2(8.0f, 2.0f);
	st.WindowBorderSize = 1.0f;
	//st.TabBorderSize = 1.0f;
	st.WindowRounding = 1.0f;
	st.ChildRounding = 1.0f;
	st.FrameRounding = 1.0f;
	st.ScrollbarRounding = 1.0f;
	st.GrabRounding = 1.0f;
	//st.TabRounding = 1.0f;

	// Setup style
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.12f, 0.25f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.53f, 0.53f, 0.53f, 0.46f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.53f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.47f, 0.47f, 0.91f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.55f, 0.55f, 0.62f);
	colors[ImGuiCol_Button] = ImVec4(0.50f, 0.50f, 0.50f, 0.63f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.67f, 0.68f, 0.63f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.26f, 0.26f, 0.63f);
	colors[ImGuiCol_Header] = ImVec4(0.54f, 0.54f, 0.54f, 0.58f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.64f, 0.65f, 0.65f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
	colors[ImGuiCol_Separator] = ImVec4(0.58f, 0.58f, 0.58f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.87f, 0.87f, 0.87f, 0.53f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	//colors[ImGuiCol_Tab] = ImVec4(0.01f, 0.01f, 0.01f, 0.86f);
	//colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	//colors[ImGuiCol_TabActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	//colors[ImGuiCol_TabUnfocused] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	//colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	//colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);
	//colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.77f, 0.33f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87f, 0.55f, 0.08f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47f, 0.60f, 0.76f, 0.47f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	//colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	//colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void TurbineRep::ProcessEvent(const SDL_Event& event)
{
	if (event.type == SDL_MOUSEMOTION && ImGui::GetIO().WantCaptureMouse == false)
	{
		const SDL_MouseMotionEvent* ev = reinterpret_cast<const SDL_MouseMotionEvent*>(&event);
		if ((ev->state & SDL_BUTTON_LMASK) > 0)
		{
			m_pAtlas->OnMouseDrag(ev->xrel, ev->yrel);
		}
	}
	else if (event.type == SDL_MOUSEWHEEL && ImGui::GetIO().WantCaptureMouse == false)
	{
		const SDL_MouseWheelEvent* ev = reinterpret_cast<const SDL_MouseWheelEvent*>(&event);
		if (ev->y > 0)
		{
			m_pAtlas->OnZoomIn();
		}
		else if (ev->y <= 0)
		{
			m_pAtlas->OnZoomOut();
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN && ImGui::GetIO().WantCaptureMouse == false)
	{
		m_SelectCamera = true;
	}
	else if (event.type == SDL_WINDOWEVENT)
	{
		if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		{
			if (event.window.windowID == 1)
			{
				m_pAtlas->OnWindowSizeChanged(event.window.data1, event.window.data2);
			}
		}
	}
}

void TurbineRep::Update(float delta)
{
	static const float sBaseCellSize = 128.0f;
	m_CellSize = sBaseCellSize;

	m_pAtlas->Update(delta);

    for (auto& cameraDisplay : m_CameraReps)
    {
        cameraDisplay.Update();
    }
}

const ImColor& TurbineRep::GetPinColor(Camera::State state) const
{
	return m_PinColor[static_cast<size_t>(state)];
}

void TurbineRep::Render()
{
	unsigned int flags = 0;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoSavedSettings;
	flags |= ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	flags |= ImGuiWindowFlags_NoInputs;
	flags |= ImGuiWindowFlags_NoScrollbar;

	ImGuiIO& io = ImGui::GetIO();
	ImTextureID my_tex_id = io.Fonts->TexID;

	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(m_pWindow, &windowWidth, &windowHeight);
	ImVec2 windowSize = ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight));

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
	ImGui::SetNextWindowSize(windowSize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Turbine", nullptr, flags);
	ImGui::PopStyleVar();

	//m_pAtlas->Render();

	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + windowSize.x, p0.y + windowSize.y);
	ImU32 colorA = ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));
	ImU32 colorB = ImGui::GetColorU32(IM_COL32(17, 43, 60, 255));
	pDrawList->AddRectFilledMultiColor(p0, p1, colorA, colorA, colorB, colorB);

    m_pMainMenuBar->Render();
	ImGui::End();

	OpenPickedCamera();
	RenderCameras();
	FlushClosedCameras();
}

void TurbineRep::OpenPickedCamera()
{
	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		// TODO: This needs to support multiple overlapping cameras, with a dropdown menu to choose from.
		CameraVector hoveredCameras = GetHoveredCameras();
		for (CameraSharedPtr& pCamera : hoveredCameras)
		{
			GeolocationData* pGeo = pCamera->GetGeolocationData();
			if (pGeo == nullptr)
			{
				ImGui::SetTooltip("%s", pCamera->GetAddress().ToString().c_str());
			}
			else
			{
				ImGui::SetTooltip("%s (%s, %s)", pCamera->GetAddress().ToString().c_str(), pGeo->GetCity().c_str(), pGeo->GetCountry().c_str());
			}
		}

		if (hoveredCameras.size() > 0 && m_SelectCamera)
		{
			bool found = false;
			for (auto& cameraDisplay : m_CameraReps)
			{
				CameraSharedPtr pCamera = cameraDisplay.GetCameraWeakPtr().lock();
				if (pCamera != nullptr && pCamera->GetURL() == hoveredCameras.front()->GetURL())
				{
					found = true;
					break;
				}
			}

			if (found == false)
			{
				m_CameraReps.emplace_back(hoveredCameras.front());

				json message =
				{
					{ "type", "stream_request" },
					{ "url", hoveredCameras.front()->GetURL() },
					{ "texture_id", m_CameraReps.back().GetTexture() }
				};
				//g_pTurbine->OnMessageReceived(message);
			}
		}

		m_SelectCamera = false;
	}
}

void TurbineRep::RenderCameras()
{
	for (auto& cameraDisplay : m_CameraReps)
	{
		cameraDisplay.Render();
	}
}

void TurbineRep::FlushClosedCameras()
{
	auto ifClosed = [&](const CameraRep& cameraRep) -> bool
	{
		return !cameraRep.IsOpen();
	};

	m_CameraReps.remove_if(ifClosed);
}

CameraVector TurbineRep::GetHoveredCameras()
{
	CameraVector hoveredCameras;
	// CameraVector cameras = g_pTurbine->GetCameras();

	// int mx, my;
	// SDL_GetMouseState(&mx, &my);

	// for (CameraSharedPtr pCamera : cameras)
	// {
	// 	GeolocationData* pGeolocationData = pCamera->GetGeolocationData();
	// 	if (pGeolocationData != nullptr)
	// 	{
	// 		float locationX, locationY;
	// 		m_pAtlas->GetScreenCoordinates(pGeolocationData->GetLongitude(), pGeolocationData->GetLatitude(), locationX, locationY);

	// 		if (mx > locationX - sPinHalfWidth && mx < locationX + sPinHalfWidth && 
	// 			my > locationY - sPinHeight && my < locationY)
	// 		{
	// 			hoveredCameras.push_back(pCamera);
	// 		}
	// 	}
	// }

	return hoveredCameras;
}

} // namespace Turbine
