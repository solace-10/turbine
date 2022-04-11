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

#include <sstream>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <implot/implot.h>

#include "bridge/bridge.h"
#include "bridge/bridgestats.hpp"
#include "providers/provider.h"
#include "windows/graphs/connectionsgraph.hpp"
#include "windows/graphs/distributionmethodgraph.hpp"
#include "windows/graphs/percountrygraph.hpp"
#include "windows/overviewwindow.hpp"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

OverviewWindow::OverviewWindow() :
m_RebuildGraphs(false)
{
    
}

OverviewWindow::~OverviewWindow()
{
    
}

void OverviewWindow::Render()
{
	if (IsOpen() == false)
	{
		return;
	}

    if (m_RebuildGraphs)
    {
        RebuildGraphs();
    }

	ImGui::SetNextWindowSize(ImVec2(650, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Overview", &m_IsOpen))
	{
		ImGui::End();
		return;
	}

    if (m_pConnectionsGraph != nullptr)
    {
        m_pConnectionsGraph->Render();
    }

    if (m_pDistributionMethodGraph != nullptr)
    {
        m_pDistributionMethodGraph->Render();
    }

	ImGui::End();
}

void OverviewWindow::OnBridgeAdded()
{
    m_RebuildGraphs = true;
}

void OverviewWindow::RebuildGraphs()
{
    m_RebuildGraphs = false;

    std::vector<BridgeStatsSharedPtr> allStats;
    const BridgeList& bridges = g_pTurbine->GetBridges();
    allStats.reserve(bridges.size());
    for (auto& pBridge : bridges)
    {
        allStats.push_back(pBridge->GetStats());
    }

    m_pConnectionsGraph = std::make_unique<ConnectionsGraph>(allStats);
    m_pDistributionMethodGraph = std::make_unique<DistributionMethodGraph>(allStats);
}

} // namespace Turbine
