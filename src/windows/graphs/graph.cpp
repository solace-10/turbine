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

#include <chrono>
#include <sstream>

#include "bridge/bridgestats.hpp"
#include "windows/graphs/graph.hpp"

namespace Turbine
{
    
Graph::Graph(BridgeStatsSharedPtr& pBridgeStats)
{
    m_DomainX[0] = m_DomainX[1] = 0.0;
    m_DomainY[0] = m_DomainY[1] = 0.0;
    m_Stats.push_back(pBridgeStats);
    m_LastStatsVersion.push_back(0);
}

Graph::Graph(std::vector<BridgeStatsSharedPtr>& bridgeStats)
{
    m_DomainX[0] = m_DomainX[1] = 0.0;
    m_DomainY[0] = m_DomainY[1] = 0.0;

    for (BridgeStatsSharedPtr& pBridgeStats : bridgeStats)
    {
        m_Stats.push_back(pBridgeStats);
        m_LastStatsVersion.push_back(0);
    }
}

void Graph::Render()
{
    bool statsChanged = true;
    const size_t numStats = m_Stats.size();
    for (size_t i = 0; i < numStats; ++i)
    {
        BridgeStatsSharedPtr pStats = m_Stats[i].lock();
        if (pStats && pStats->GetVersion() != m_LastStatsVersion[i])
        {
            m_LastStatsVersion[i] = pStats->GetVersion();
            statsChanged = true;
        }
    }

    if (statsChanged)
    {
        OnBridgeStatsChanged();
    }
}

void Graph::OnBridgeStatsChanged()
{
    m_DomainX[0] = m_DomainX[1] = 0.0;
    m_DomainY[0] = m_DomainY[1] = 0.0;
}

double Graph::ToUnixTimestamp(const std::string& date) const
{
    std::tm tm = {};
    strptime(date.c_str(), "%Y-%m-%d", &tm);
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    int64_t count = std::chrono::duration<double>(tp.time_since_epoch()).count();
    return static_cast<double>(count);
}

} // namespace Turbine