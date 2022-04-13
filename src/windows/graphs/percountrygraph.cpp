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
#include <set>

#include <imgui/imgui.h>
#include <implot/implot.h>

#include "bridge/bridgestats.hpp"
#include "windows/graphs/percountrygraph.hpp"

namespace Turbine
{
    
PerCountryGraph::PerCountryGraph(BridgeStatsSharedPtr& pBridgeStats) :
Graph(pBridgeStats),
m_Title("Per country stats"),
m_UseSimplified(false)
{
    
}

void PerCountryGraph::Render()
{
    Graph::Render();

    if (m_Dates.empty() == false && ImPlot::BeginPlot(m_Title.c_str())) 
    {        
        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_Time);
        ImPlot::SetupAxis(ImAxis_Y1, "Connections", ImPlotAxisFlags_LockMin);
        ImPlot::SetupAxesLimits(m_DomainX[0], m_DomainX[1], m_DomainY[0], m_DomainY[1]);

        if (m_UseSimplified)
        {
            for (const std::string& country : m_SimplifiedCountries)
            {
                const std::vector<double>& countryData = m_CountryData[country];
                ImPlot::PlotLine(country.c_str(), m_Dates.data(), countryData.data(), m_Dates.size());
            }
        }
        else
        {
            for (auto& perCountryData : m_CountryData)
            {
                ImPlot::PlotLine(perCountryData.first.c_str(), m_Dates.data(), perCountryData.second.data(), m_Dates.size());
            }
        }

        ImPlot::EndPlot();
    }
}

void PerCountryGraph::OnBridgeStatsChanged()
{
    Graph::OnBridgeStatsChanged();

    BridgeStatsSharedPtr pStats = m_Stats[0].lock();
    if (pStats == nullptr)
    {
        return;
    }

    const size_t numEntries = pStats->GetEntryCount();
    m_Dates.clear();
    m_Dates.resize(numEntries);
    for (size_t i = 0; i < numEntries; ++i)
    {
        const PerCountryStats& perCountryStats = pStats->GetPerCountryStats(i);
        for (auto& pair : perCountryStats)
        {
            m_AllCountries.insert(pair.first);
        }

        m_Dates[i] = ToUnixTimestamp(pStats->GetDate(i));
    }

    for (const std::string& country : m_AllCountries)
    {
        m_CountryData[country].resize(numEntries);                    
    }

    for (size_t i = 0; i < numEntries; ++i)
    {
        const PerCountryStats& perCountryStats = pStats->GetPerCountryStats(i);
        for (const std::string& country : m_AllCountries)
        {
            PerCountryStats::const_iterator it = perCountryStats.find(country);
            if (it == perCountryStats.cend())
            {
                m_CountryData[country][i] = 0.0;
            }
            else
            {
                const double value = static_cast<double>(it->second);
                m_CountryData[country][i] = value;
                m_DomainY[1] = std::max(m_DomainY[1], value);
            }
        }
    }

    m_DomainX[0] = m_Dates.front();
    m_DomainX[1] = m_Dates.back();
    m_DomainY[1] = m_DomainY[1] + 10; // Ensure we don't get clipped at the top of the graph.

    if (m_AllCountries.size() > 10)
    {
        m_Title = "Per country stats (> 8 connections)";
        m_UseSimplified = true;
        m_SimplifiedCountries.clear();
        for (auto& countryData : m_CountryData)
        {
            double max = GetCountryMax(countryData.first);
            if (max > 8.0)
            {
                m_SimplifiedCountries.push_back(countryData.first);
            }
        }
    }
    else
    {
        m_Title = "Per country stats";
        m_UseSimplified = false;
    }
}

double PerCountryGraph::GetCountryMax(const std::string& country)
{
    const std::vector<double>& values = m_CountryData[country];
    double m = 0.0;
    for (double v : values)
    {
        if (v > m)
        {
            m = v;
        }
    }
    return m;
}

} // namespace Turbine