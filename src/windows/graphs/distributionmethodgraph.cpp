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

#include <map>
#include <sstream>

#include <imgui/imgui.h>
#include <implot/implot.h>

#include "bridge/bridgestats.hpp"
#include "windows/graphs/distributionmethodgraph.hpp"
#include "turbine.h"

namespace Turbine
{
    
DistributionMethodGraph::DistributionMethodGraph(BridgeStatsSharedPtr& pBridgeStats) :
Graph(pBridgeStats)
{
    
}

DistributionMethodGraph::DistributionMethodGraph(std::vector<BridgeStatsSharedPtr>& bridgeStats) :
Graph(bridgeStats)
{

}

void DistributionMethodGraph::Render()
{
    Graph::Render();

    std::map<std::string, int> methods;
    for (Bridge* pBridge : g_pTurbine->GetBridges())
    {
        auto it = methods.find(pBridge->GetDistributionMechanism());
        if (it == methods.end())
        {
            methods[pBridge->GetDistributionMechanism()] = 1;
        }
        else
        {
            it->second++;
        }
    }

    std::vector<const char*> labels;
    std::vector<double> data;
    for (auto& pair : methods)
    {
        labels.push_back(pair.first.c_str());
        data.push_back(pair.second);
    }

    if (ImPlot::BeginPlot("Distribution methods", ImVec2(250,250), ImPlotFlags_Equal | ImPlotFlags_NoMouseText)) 
    {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(0, 1, 0, 1);
        ImPlot::PlotPieChart(labels.data(), data.data(), labels.size(), 0.5, 0.5, 0.4, true, "%.0f");
        ImPlot::EndPlot();
    }
}

void DistributionMethodGraph::OnBridgeStatsChanged()
{
    Graph::OnBridgeStatsChanged();
}

} // namespace Turbine