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

#include "providers/provider.h"
#include "windows/bridgeswindow.h"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

void BridgesWindow::Render()
{
	ImGuiViewport* pViewport = ImGui::GetMainViewport();
	int menuHeight = 17;
    ImVec2 pos(pViewport->Pos.x, pViewport->Pos.y + menuHeight);
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(ImVec2(pViewport->Size.x - 250, pViewport->Size.y - menuHeight));
	ImGui::Begin("Bridges", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
	if (ImGui::BeginTable("Bridges", 3, flags))
	{
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Type");
		ImGui::TableSetupColumn("IP");
		ImGui::TableHeadersRow();

		BridgeList pBridges = g_pTurbine->GetBridges();
		for (auto& pBridge : pBridges)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::TextUnformatted(pBridge->GetName().c_str());
			ImGui::TableNextColumn(); ImGui::TextUnformatted("Listed");
			ImGui::TableNextColumn(); ImGui::TextUnformatted(pBridge->GetIPv4().c_str());
		}

		ImGui::EndTable();
	}

	ImGui::End();
}

} // namespace Turbine
