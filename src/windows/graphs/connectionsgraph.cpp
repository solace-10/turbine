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

#include "bridge/bridgestats.hpp"
#include "windows/graphs/connectionsgraph.hpp"

namespace Turbine
{
    
ConnectionsGraph::ConnectionsGraph(BridgeStatsSharedPtr& pBridgeStats) :
Graph(pBridgeStats)
{
    
}

void ConnectionsGraph::Render()
{
    Graph::Render();

    if (m_Dates.empty() == false && ImPlot::BeginPlot("Connections")) 
    {
        
        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_Time);
        ImPlot::SetupAxesLimits(m_DomainX[0], m_DomainX[1], m_DomainY[0], m_DomainY[1]);
        ImPlot::SetupAxis(ImAxis_Y1, "Connections");
        ImPlot::PlotLine("IPv4", m_Dates.data(), m_ConnectionsIPv4.data(), m_Dates.size());
        ImPlot::PlotLine("IPv6", m_Dates.data(), m_ConnectionsIPv6.data(), m_Dates.size());
        ImPlot::EndPlot();
    }
}

void ConnectionsGraph::OnBridgeStatsChanged()
{
    Graph::OnBridgeStatsChanged();

    BridgeStatsSharedPtr pStats = m_pStats.lock();
    if (pStats == nullptr)
    {
        return;
    }

    m_Dates.clear();
    m_ConnectionsIPv4.clear();
    m_ConnectionsIPv6.clear();

    const size_t numEntries = pStats->GetEntryCount();
    m_Dates.resize(numEntries);
    m_ConnectionsIPv4.resize(numEntries);
    m_ConnectionsIPv6.resize(numEntries);
    for (size_t i = 0; i < numEntries; ++i)
    {
        m_Dates[i] = ToUnixTimestamp(pStats->GetDate(i));
        m_ConnectionsIPv4[i] = static_cast<double>(pStats->GetIpv4Stats(i));
        m_ConnectionsIPv6[i] = static_cast<double>(pStats->GetIpv6Stats(i));

        m_DomainY[1] = std::max(std::max(m_DomainY[1], m_ConnectionsIPv4[i]), m_ConnectionsIPv6[i]);
    }
    m_DomainX[0] = m_Dates.front();
    m_DomainX[1] = m_Dates.back();
    m_DomainY[1] = m_DomainY[1] + 10; // Ensure we don't get clipped at the top of the graph.
}

} // namespace Turbine