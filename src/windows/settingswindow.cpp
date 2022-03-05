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

#include "windows/settingswindow.h"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

void SettingsWindow::Render()
{
	if (IsOpen() == false)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(650, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Settings", &m_IsOpen))
	{
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Providers", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::TreeNodeEx("Digital Ocean", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static std::string apiKey = g_pTurbine->GetSettings()->GetDigitalOceanAPIKey();
			if (ImGui::InputText("Personal Access Token", &apiKey))
			{
				g_pTurbine->GetSettings()->SetDigitalOceanAPIKey(apiKey);
			}
			ImGui::SameLine();
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted("In order to use Digital Ocean with Turbine, you need to provide a Personal Access Token. The token must have Read and Write scopes.");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
}

} // namespace Turbine
