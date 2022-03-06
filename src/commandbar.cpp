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

#include <imgui/imgui.h>

#include "atlas/tilestreamer.h"
#include "tasks/googlesearch/googlesearch.h"
#include "tasks/task.h"
#include "windows/settingswindow.h"
#include "commandbar.h"
#include "turbine.h"

namespace Turbine
{

CommandBar::CommandBar()
{
    m_AnimTimer = 0.0f;
    m_ShowAtlasTileStreamer = false;
    m_ShowDemoWindow = false;
}

void CommandBar::Render()
{
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
	ImGui::SetNextWindowSize(ImVec2(250, 0));
	ImGui::Begin("CommandBar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            bool bridgesWindowOpen = g_pTurbine->GetBridgesWindow()->IsOpen();
            if (ImGui::MenuItem("Bridges", nullptr, &bridgesWindowOpen))
            {
                g_pTurbine->GetBridgesWindow()->Show(bridgesWindowOpen);
            }

            bool settingsWindowOpen = g_pTurbine->GetSettingsWindow()->IsOpen();
            if (ImGui::MenuItem("Settings", nullptr, &settingsWindowOpen))
            {
                g_pTurbine->GetSettingsWindow()->Show(settingsWindowOpen);
            }

			if (ImGui::BeginMenu("Development"))
			{
				if (ImGui::MenuItem("Atlas tile streamer", nullptr, m_ShowAtlasTileStreamer))
				{
					m_ShowAtlasTileStreamer = !m_ShowAtlasTileStreamer;
				}
				if (ImGui::MenuItem("ImGui demo window", nullptr, m_ShowDemoWindow))
				{
					m_ShowDemoWindow = !m_ShowDemoWindow;
				}
				ImGui::EndMenu();
			}

            ImGui::Separator();
            ImGui::MenuItem("Quit", nullptr, nullptr, false);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("About", nullptr, nullptr, false);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (m_ShowAtlasTileStreamer)
    {
        TileStreamer::ShowDebugUI(&m_ShowAtlasTileStreamer);
    }

    if (m_ShowDemoWindow)
    {
        ImGui::ShowDemoWindow(&m_ShowDemoWindow);
    }

    RenderSearchWidget();
    RenderTasks();
    ImGui::End();
}

void CommandBar::RenderSearchWidget()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
    ImGui::BeginChild("SearchWidget", ImVec2(0.0f, 30.0f), true);
    RenderSearchBackground();
    RenderSearchButton();
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

void CommandBar::RenderSearchBackground()
{
    const bool isSearching = true;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* pDrawList = ImGui::GetWindowDrawList();

    const float width = ImGui::GetWindowWidth();
    const float height = ImGui::GetWindowHeight();

    ImVec2 tl(p);
    ImVec2 br(p.x + width, p.y + height);
    ImU32 bg = IM_COL32(128, 64, 0, 255);
    ImU32 fg = IM_COL32(255, 128, 0, 255);
    pDrawList->AddRectFilled(tl, br, bg);

    const float stripeWidth = 15.0f;
    const float stripeSlant = 15.0f;
    for (int i = 0; i < 14; ++i)
    {
        float xOffset = 30.0f * (i - 1) + m_AnimTimer;
        pDrawList->AddQuadFilled(
            ImVec2(tl.x + xOffset, tl.y),
            ImVec2(tl.x + xOffset + stripeWidth, tl.y),
            ImVec2(tl.x + xOffset + stripeWidth + stripeSlant, br.y),
            ImVec2(tl.x + xOffset + stripeSlant, br.y),
            fg
        );
    }

    m_AnimTimer += ImGui::GetIO().DeltaTime * 5.0f;
    if (m_AnimTimer > stripeWidth * 2.0f)
    {
        m_AnimTimer = 0.0f;
    }
}

void CommandBar::RenderSearchButton()
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* pDrawList = ImGui::GetWindowDrawList();

    if (ImGui::InvisibleButton("SearchWidgetButton", ImGui::GetWindowSize()))
    {
        g_pTurbine->GetCreateBridgeWindow()->Show(true);
    }

    const float width = ImGui::GetWindowWidth();
    const float height = ImGui::GetWindowHeight();
    const ImVec2 padding(20, 6);
    const ImVec2 tl(p.x + padding.x, p.y + padding.y);
    const ImVec2 br(p.x + width - padding.x, p.y + height - padding.y);

    ImU32 borderColor = ImGui::IsItemHovered() ? IM_COL32(200, 200, 200, 255) : IM_COL32(128, 128, 128, 255);
    pDrawList->AddRectFilled(tl, br, IM_COL32(20, 20, 20, 200));
    pDrawList->AddRect(tl, br, borderColor);

    const std::string& text("Create bridge");
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    pDrawList->AddText(ImVec2(p.x + (width - textSize.x) / 2.0f, p.y + (height - ImGui::GetTextLineHeight()) / 2.0f), IM_COL32(255, 255, 255, 255), text.c_str());
}

void CommandBar::RenderTasks()
{
    const TaskVector& tasks = g_pTurbine->GetTasks();
    for (auto&& pTask : tasks)
    {
        pTask->Render();
    }
}

} // namespace Turbine
