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

#include <array>
#include <filesystem>
#include <fstream>

#include "bridge/bridge.h"
#include "bridge/bridgestats.hpp"
#include "core/stringops.hpp"

namespace Turbine
{

BridgeStats::BridgeStats(Bridge* pBridge) :
m_pBridge(pBridge)
{
    // The raw file contains the bridge-stats file which we download from the bridge via Ansible. 
    // This file only contains stats for the last 24h.
    m_BridgeStatsRawPath = pBridge->GetStoragePath() / "bridge-stats";

    // The archive file contains the aggregated statistics over a period of time.
    // New data is appended to it when the raw file contains updated stats.
    m_BridgeStatsArchivePath = pBridge->GetStoragePath() / "bridgestats.json";
}

BridgeStats::~BridgeStats()
{

}

void BridgeStats::OnMonitoredDataUpdated()
{
    using namespace StringOps;
    std::ifstream file(m_BridgeStatsRawPath, std::ifstream::in);
    if (file.good())
    {
        Entry entry;

        std::string line;
        while (getline(file, line, '\n'))
        {
            if (BeginsWith(line, "bridge-stats-end"))
            {
                ParseBridgeStatsEndLine(line, &entry);
            }
            else if (BeginsWith(line, "bridge-ips"))
            {
                ParseBridgeIpsLine(line, &entry);
            }
            else if (BeginsWith(line, "bridge-ip-version"))
            {
                ParseBridgeIpVersionsLine(line, &entry);
            }
        }

        file.close();
    }
}

void BridgeStats::ParseBridgeStatsEndLine(const std::string& line, Entry* pEntry)
{
    std::vector<std::string> tokens = StringOps::Split(line, ' ');
    if (tokens.size() >= 2)
    {
        pEntry->date = tokens[1];
    }
}

void BridgeStats::ParseBridgeIpsLine(const std::string& line, Entry* pEntry)
{
    using namespace StringOps;

    // Format is "bridge-ips de=8,ru=8"
    Tokens tokens = Split(line, ' '); // { "bridge-ips", "de=8,ru=8"}
    if (tokens.size() >= 2)
    {
        Tokens countries = Split(tokens[1], ','); // { "de=8", "ru=8" }
        for (const std::string& country : countries)
        {
            Tokens pair = Split(country, '='); // { "de", "8" }
            if (pair.size() == 2)
            {
                const std::string& countryCode = pair[0];
                int count = atoi(pair[1].c_str());
                pEntry->usage[countryCode] = count;
            } 
        }
    }
}

void BridgeStats::ParseBridgeIpVersionsLine(const std::string& line, Entry* pEntry)
{
    using namespace StringOps;

    // Format is "bridge-ip-versions v4=8,v6=0"
    Tokens tokens = Split(line, ' '); // { "bridge-ip-versions", "v4=8,v6=0"}
    if (tokens.size() >= 2)
    {
        Tokens versions = Split(tokens[1], ','); // { "v4=8", "v6=0" }
        for (const std::string& version : versions)
        {
            Tokens pair = Split(version, '='); // { "v4", "8" }
            if (pair.size() == 2)
            {
                int value = atoi(pair[1].c_str());
                if (pair[0] == "v4")
                {
                    pEntry->v4 = value;
                }
                else if (pair[0] == "v6")
                {
                    pEntry->v6 = value;
                }
            } 
        }
    }
}

} // namespace Turbine