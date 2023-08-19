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
#include "fonts.h"
#include "providers/provider.h"
#include "windows/bridgeswindow.h"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

void BridgesWindow::Render()
{
	return;
	using namespace ImGui;

	ImGuiViewport* pViewport = GetMainViewport();
	int menuHeight = 22;
    ImVec2 pos(pViewport->Pos.x, pViewport->Pos.y + menuHeight);
	SetNextWindowPos(pos);
	SetNextWindowSize(ImVec2(pViewport->Size.x - 250, pViewport->Size.y - menuHeight));
	
	ImGuiWindowFlags flags = 0 |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoBackground;
	Begin("Bridges", nullptr, flags);

    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	BridgeList pBridges = g_pTurbine->GetBridges();
	const ImVec2 childSize(180, 180);
	const ImVec2 serverIconSize(64, 64);
	for (auto& pBridge : pBridges)
	{
		const bool hasError = pBridge->GetError();
		if (hasError)
		{
			PushStyleColor(ImGuiCol_Border, IM_COL32(128, 0, 0, 255));
		}
		else
		{
			PushStyleColor(ImGuiCol_Border, IM_COL32(84, 163, 220, 255));
		}

		BeginChild(pBridge->GetId().c_str(), childSize, true);

		if (hasError)
		{
			ImDrawList* pDrawList = ImGui::GetWindowDrawList();
			ImVec2 p0 = ImGui::GetCursorScreenPos();
			ImVec2 p1 = ImVec2(p0.x + childSize.x, p0.y + childSize.y);
			ImU32 colorA = ImGui::GetColorU32(IM_COL32(255, 0, 0, 0));
			ImU32 colorB = ImGui::GetColorU32(IM_COL32(255, 0, 0, 20));
			pDrawList->AddRectFilledMultiColor(p0, p1, colorA, colorA, colorB, colorB);
		}

		ImVec2 pos = GetCursorScreenPos();
		
		SetCursorPos(ImVec2(childSize.x / 2 - serverIconSize.x / 2, 16));		
		if (hasError)
		{
			Image(reinterpret_cast<ImTextureID>(Icons::GetIcon(IconId::Server)), serverIconSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 0, 0, 1));
			PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		}
		else
		{
			Image(reinterpret_cast<ImTextureID>(Icons::GetIcon(IconId::Server)), serverIconSize);
			PushStyleColor(ImGuiCol_Text, IM_COL32(84, 163, 220, 255));
		}

		PushFont(Fonts::GetFont(FontId::AltoVoltaje32));
		const ImVec2 nameSize = CalcTextSize(pBridge->GetName().c_str());
		SetCursorScreenPos(ImVec2(pos.x + childSize.x / 2 - nameSize.x / 2, pos.y + 88));
		TextUnformatted(pBridge->GetName().c_str());
		PopFont();

		PushFont(Fonts::GetFont(FontId::Inconsolata18));
		const ImVec2 stateSize = CalcTextSize(pBridge->GetStateText().c_str());
		SetCursorScreenPos(ImVec2(pos.x + childSize.x / 2 - stateSize.x / 2, pos.y + 96 + nameSize.y));
		TextUnformatted(pBridge->GetStateText().c_str());

		if (!hasError && pBridge->GetTorState() == Bridge::TorState::Running && pBridge->GetUniqueClients().has_value())
		{
			std::stringstream clientsText;
			clientsText << "~" << pBridge->GetUniqueClients().value() << " clients";
			const ImVec2 clientsSize = CalcTextSize(clientsText.str().c_str());
			SetCursorScreenPos(ImVec2(pos.x + childSize.x / 2 - clientsSize.x / 2, pos.y + 96 + nameSize.y + stateSize.y));
			TextUnformatted(clientsText.str().c_str());
		}
		PopFont();

		PopStyleColor();

		SetCursorScreenPos(pos);
		if (InvisibleButton("BridgeSummaryButton", GetWindowSize()))
		{
			Window* pWindow = g_pTurbine->GetBridgeWindow(pBridge);
			if (pWindow != nullptr)
			{
				pWindow->Show(true);
			}
		}
		ImGui::PopStyleColor();

		EndChild();
		SameLine();
	}

    PopStyleVar();
	End();
}

} // namespace Turbine
