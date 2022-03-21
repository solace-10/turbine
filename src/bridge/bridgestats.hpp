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

#pragma once

#include <filesystem>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "bridge/bridge.fwd.hpp"

namespace Turbine
{

class BridgeStats
{
public:
    BridgeStats(Bridge* pBridge);
    ~BridgeStats();

    void OnMonitoredDataUpdated();

private:

    struct Entry
    {
        Entry()
        {
            v4 = 0;
            v6 = 0;
        }

        std::string date;
        std::unordered_map<std::string, int> usage;
        int v4;
        int v6;
    };
    using EntryVector = std::vector<Entry>;

    void ParseBridgeStatsEndLine(const std::string& line, Entry* pEntry);
    void ParseBridgeIpsLine(const std::string& line, Entry* pEntry);
    void ParseBridgeIpVersionsLine(const std::string& line, Entry* pEntry);

    Bridge* m_pBridge;
    std::filesystem::path m_BridgeStatsArchivePath;
    std::filesystem::path m_BridgeStatsRawPath;
    EntryVector m_Entries;
};

} // namespace Turbine