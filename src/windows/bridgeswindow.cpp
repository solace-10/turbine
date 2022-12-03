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

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include "icons.h"
#include "providers/provider.h"
#include "windows/bridgeswindow.h"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

void BridgesWindow::Render()
{
	ImGuiViewport* pViewport = ImGui::GetMainViewport();
	int menuHeight = 22;
    ImVec2 pos(pViewport->Pos.x, pViewport->Pos.y + menuHeight);
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(ImVec2(pViewport->Size.x - 250, pViewport->Size.y - menuHeight));
	ImGui::Begin("Bridges", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

    using namespace ImGui;
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	BridgeList pBridges = g_pTurbine->GetBridges();
	const ImVec2 childSize(160, 160);
	const ImVec2 serverIconSize(64, 64);
	for (auto& pBridge : pBridges)
	{
		BeginChild(pBridge->GetId().c_str(), childSize, true);

		ImVec2 pos = GetCursorScreenPos();
		
		SetCursorPos(ImVec2(childSize.x / 2 - serverIconSize.x / 2, 16));
		Image(reinterpret_cast<ImTextureID>(Icons::GetIcon(IconId::Server)), serverIconSize);
		
		SetCursorScreenPos(ImVec2(pos.x + 8, pos.y + 4));
		//SpinnerFilled(10.0f, 4, IM_COL32(0, 255, 255, 255), &timer);

		const ImVec2 nameSize = CalcTextSize(pBridge->GetName().c_str());
		SetCursorScreenPos(ImVec2(pos.x + childSize.x / 2 - nameSize.x / 2, pos.y + 88));
		TextUnformatted(pBridge->GetName().c_str());

		const ImVec2 stateSize = CalcTextSize(pBridge->GetState().c_str());
		SetCursorScreenPos(ImVec2(pos.x + childSize.x / 2 - stateSize.x / 2, pos.y + 96 + nameSize.y));
		TextUnformatted(pBridge->GetState().c_str());

		SetCursorScreenPos(pos);
		if (ImGui::InvisibleButton("BridgeSummaryButton", ImGui::GetWindowSize()))
		{
			Window* pWindow = g_pTurbine->GetBridgeWindow(pBridge);
			if (pWindow != nullptr)
			{
				pWindow->Show(true);
			}
		}

		EndChild();
		SameLine();
	}

    PopStyleVar();

	ImGui::End();
}

} // namespace Turbine
