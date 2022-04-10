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
#include <implot/implot.h>

#include "atlas/tilestreamer.h"
#include "windows/settingswindow.h"
#include "mainmenubar.hpp"
#include "turbine.h"

namespace Turbine
{

MainMenuBar::MainMenuBar()
{
    m_ShowAtlasTileStreamer = false;
    m_ShowImGuiDemoWindow = false;
    m_ShowImPlotDemoWindow = false;
}

void MainMenuBar::Render()
{
    if (ImGui::BeginMainMenuBar())
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

                bool deploymentWindowOpen = g_pTurbine->GetDeploymentWindow()->IsOpen();
                if (ImGui::MenuItem("Deployments", nullptr, &deploymentWindowOpen))
                {
                    g_pTurbine->GetDeploymentWindow()->Show(deploymentWindowOpen);
                }

				if (ImGui::MenuItem("ImGui demo window", nullptr, m_ShowImGuiDemoWindow))
				{
					m_ShowImGuiDemoWindow = !m_ShowImGuiDemoWindow;
				}

                if (ImGui::MenuItem("ImPlot demo window", nullptr, m_ShowImPlotDemoWindow))
                {
                    m_ShowImPlotDemoWindow = !m_ShowImPlotDemoWindow;
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

        ImGui::EndMainMenuBar();
    }

    if (m_ShowAtlasTileStreamer)
    {
        TileStreamer::ShowDebugUI(&m_ShowAtlasTileStreamer);
    }

    if (m_ShowImGuiDemoWindow)
    {
        ImGui::ShowDemoWindow(&m_ShowImGuiDemoWindow);
    }

    if (m_ShowImPlotDemoWindow)
    {
        ImPlot::ShowDemoWindow(&m_ShowImPlotDemoWindow);
    }
}

} // namespace Turbine
