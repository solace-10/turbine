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
#include <memory>
#include <string>

#include "bridge/bridge.fwd.hpp"
#include "bridge/bridgesummarywidget.h"

namespace Turbine
{

class BridgeGeolocation;
using BridgeGeolocationUniquePtr = std::unique_ptr<BridgeGeolocation>;

class BridgeSummaryWidget;
using BridgeSummaryWidgetUniquePtr = std::unique_ptr<BridgeSummaryWidget>;

class Provider;

class StateMachine;
using StateMachineUniquePtr = std::unique_ptr<StateMachine>;

class Bridge
{
public:
    Bridge(Provider* pProvider, const std::string& id, const std::string& name, const std::string& state);
    ~Bridge();

    Provider* GetProvider() const;
    const std::string& GetId() const;
    const std::string& GetName() const;
    const std::string& GetState() const;
    const std::string& GetIPv4() const;
    const std::string& GetIPv6() const;
    void SetIPv4(const std::string& ip);
    void SetIPv6(const std::string& ip);
    unsigned int GetORPort() const;
    unsigned int GetExtPort() const;
    void SetORPort(unsigned int port);
    void SetExtPort(unsigned int port);
    void SetState(const std::string& state, bool force = false);
    std::filesystem::path GetStoragePath() const;
    const std::string& GetFingerprint() const;
    const std::string& GetHashedFingerprint() const;
    const BridgeStatsSharedPtr& GetStats() const;
    BridgeStatsSharedPtr& GetStats();
    const std::string& GetDistributionMechanism() const;
    BridgeGeolocation* GetGeolocation() const;

    void OnMonitoredDataUpdated();

    void RenderSummaryWidget();

private:
    void InitialiseStateMachine();
    std::string ReadFingerprint(const std::filesystem::path filePath) const;
    void ReadFingerprint();
    void ReadHashedFingerprint();
    void ReadBridgeStats();
    void RetrieveDistributionMechanism();

    Provider* m_pProvider;
    std::string m_Id;
    std::string m_Name;
    BridgeSummaryWidgetUniquePtr m_pBridgeSummaryWidget;
    StateMachineUniquePtr m_pStateMachine;
    std::string m_Ipv4;
    std::string m_Ipv6;
    unsigned int m_ORPort;
    unsigned int m_ExtPort;
    std::string m_Fingerprint;
    std::string m_HashedFingerprint;
    BridgeStatsSharedPtr m_pBridgeStats;
    std::filesystem::path m_BridgePath;
    std::string m_DistributionMechanism;
    BridgeGeolocationUniquePtr m_pGeolocation;
};

} // namespace Turbine