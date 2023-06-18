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
#include <optional>
#include <string>
#include <vector>

#include "bridge/bridge.fwd.hpp"
#include "bridge/bridgeissue.hpp"
#include "bridge/bridgesummarywidget.h"

namespace Turbine
{

class BridgeGeolocation;
using BridgeGeolocationUniquePtr = std::unique_ptr<BridgeGeolocation>;

using BridgeIssueUniquePtr = std::unique_ptr<BridgeIssue>;
using BridgeIssues = std::vector<BridgeIssueUniquePtr>;

class BridgeSummaryWidget;
using BridgeSummaryWidgetUniquePtr = std::unique_ptr<BridgeSummaryWidget>;

class Provider;

class Bridge
{
public:
    enum class VPSState
    {
        Active,
        New,
        Offline,
        Unknown,

        Count
    };

    enum class DeploymentState
    {
        Unknown,
        DeploymentPending,
        Deploying,
        Deployed,
        DeploymentFailed,
        Unreachable,

        Count
    };

    enum class TorState
    {
        Unknown,
        Unreachable,
        Inactive,
        Running,

        Count
    };

    Bridge(Provider* pProvider, const std::string& id, const std::string& name, VPSState vpsState, DeploymentState deploymentState, TorState torState);
    ~Bridge();

    Provider* GetProvider() const;
    const std::string& GetId() const;
    const std::string& GetName() const;
    const std::string& GetIPv4() const;
    const std::string& GetIPv6() const;
    void SetIPv4(const std::string& ip);
    void SetIPv6(const std::string& ip);
    unsigned int GetORPort() const;
    unsigned int GetExtPort() const;
    void SetORPort(unsigned int port);
    void SetExtPort(unsigned int port);
    std::filesystem::path GetStoragePath() const;
    const std::string& GetFingerprint() const;
    const std::string& GetHashedFingerprint() const;
    const BridgeStatsSharedPtr& GetStats() const;
    BridgeStatsSharedPtr& GetStats();
    const std::string& GetDistributionMechanism() const;
    BridgeGeolocation* GetGeolocation() const;
    VPSState GetVPSState() const;
    void SetVPSState(VPSState state);
    DeploymentState GetDeploymentState() const;
    void SetDeploymentState(DeploymentState state);
    TorState GetTorState() const;
    void SetTorState(TorState state);
    std::string GetStateText() const;
    void SetError(bool errorState, const std::string& detail);
    bool GetError() const;
    const std::string& GetErrorDetail() const;
    std::optional<int> GetUniqueClients() const;
    const BridgeIssues& GetBridgeIssues() const;

    void OnMonitoredDataUpdated();

    void RenderSummaryWidget();

private:
    std::string ReadFingerprint(const std::filesystem::path filePath) const;
    void ReadObfs4ProxyVersion();
    void ReadTorProcess();
    void ReadTorVersion();
    void ReadFingerprint();
    void ReadHashedFingerprint();
    void ReadBridgeStats();
    void ReadHeartbeatUsers();
    void RetrieveDistributionMechanism();
    void AddIssue(BridgeIssue::Type type, const std::string& description);
    void ClearIssues();

    Provider* m_pProvider;
    std::string m_Id;
    std::string m_Name;
    BridgeSummaryWidgetUniquePtr m_pBridgeSummaryWidget;
    std::string m_Ipv4;
    std::string m_Ipv6;
    unsigned int m_ORPort;
    unsigned int m_ExtPort;
    std::string m_Obfs4ProxyVersion;
    std::string m_TorVersion;
    std::string m_Fingerprint;
    std::string m_HashedFingerprint;
    BridgeStatsSharedPtr m_pBridgeStats;
    std::filesystem::path m_BridgePath;
    std::string m_DistributionMechanism;
    BridgeGeolocationUniquePtr m_pGeolocationIPv4;
    BridgeGeolocationUniquePtr m_pGeolocationIPv6;
    VPSState m_VPSState;
    DeploymentState m_DeploymentState;
    TorState m_TorState;
    bool m_Error;
    std::string m_ErrorDetail;
    std::optional<int> m_UniqueClients;
    BridgeIssues m_Issues;
};

} // namespace Turbine