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
#include <fstream>
#include <regex>

#include "bridge/bridge.h"
#include "bridge/bridgegeolocation.hpp"
#include "bridge/bridgestats.hpp"
#include "bridge/bridgesummarywidget.h"
#include "deployment/deployment.h"
#include "webclient/webclient.h"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

Bridge::Bridge(Provider* pProvider, const std::string& id, const std::string& name, VPSState vpsState, DeploymentState deploymentState, TorState torState) :
m_pProvider(pProvider),
m_Id(id),
m_Name(name),
m_ORPort(0),
m_ExtPort(0),
m_DistributionMechanism("unknown"),
m_VPSState(vpsState),
m_DeploymentState(deploymentState),
m_TorState(torState),
m_Error(false)
{
    m_pBridgeSummaryWidget = std::make_unique<BridgeSummaryWidget>();
    m_BridgePath = g_pTurbine->GetSettings()->GetStoragePath() / "bridges" / GetName();
    m_pBridgeStats = std::make_shared<BridgeStats>(this);
}

Bridge::~Bridge()
{
}

Provider* Bridge::GetProvider() const
{
    return m_pProvider;
}

const std::string& Bridge::GetId() const
{
    return m_Id;
}

const std::string& Bridge::GetName() const
{
    return m_Name;
}

const std::string& Bridge::GetIPv4() const
{
    return m_Ipv4;
}

const std::string& Bridge::GetIPv6() const
{
    return m_Ipv6;
}

void Bridge::SetIPv4(const std::string& ip)
{
    m_Ipv4 = ip;
    m_pGeolocationIPv4 = std::make_unique<BridgeGeolocation>(ip);
}

void Bridge::SetIPv6(const std::string& ip)
{
    m_Ipv6 = ip;
    m_pGeolocationIPv6 = std::make_unique<BridgeGeolocation>(ip);
}

unsigned int Bridge::GetORPort() const
{
    return m_ORPort;
}

unsigned int Bridge::GetExtPort() const
{
    return m_ExtPort;
}

void Bridge::SetORPort(unsigned int port)
{
    m_ORPort = port;
}

void Bridge::SetExtPort(unsigned int port)
{
    m_ExtPort = port;
}

// Returns the location in the filesystem where we store
// any files which are specific to this bridge.
std::filesystem::path Bridge::GetStoragePath() const
{
    return m_BridgePath;
}

const std::string& Bridge::GetFingerprint() const
{
    return m_Fingerprint;
}

const std::string& Bridge::GetHashedFingerprint() const
{
    return m_HashedFingerprint;
}

const BridgeStatsSharedPtr& Bridge::GetStats() const
{
    return m_pBridgeStats;
}

BridgeStatsSharedPtr& Bridge::GetStats()
{
    return m_pBridgeStats;
}

const std::string& Bridge::GetDistributionMechanism() const
{
    return m_DistributionMechanism;
}

BridgeGeolocation* Bridge::GetGeolocation() const
{
    return m_pGeolocationIPv4 ? m_pGeolocationIPv4.get() : m_pGeolocationIPv6.get();
}

void Bridge::RenderSummaryWidget()
{
    m_pBridgeSummaryWidget->Render(this);
}

void Bridge::OnMonitoredDataUpdated()
{
    ReadObfs4ProxyVersion();
    ReadTorProcess();
    ReadTorVersion();
    ReadBridgeStats();
    ReadFingerprint();
    ReadHashedFingerprint();
    ReadHeartbeatUsers();
    RetrieveDistributionMechanism();
}

void Bridge::ReadObfs4ProxyVersion()
{
    std::ifstream file(m_BridgePath / "obfs4proxy-version", std::ifstream::in);
    if (file.good())
    {
        std::string version;
        if (getline(file, version))
        {
            std::string prefix("obfs4proxy-");
            if (version.length() > prefix.length())
            {
                m_Obfs4ProxyVersion = version.substr(prefix.length());
            }

            if (m_Obfs4ProxyVersion != g_pTurbine->GetLatestObfs4ProxyVersion())
            {
                Log::Warning("%s: outdated obfs4proxy!", GetName().c_str());
            }
        }
        else
        {
            SetError(true, "Failed to read obfs4proxy-version file.");
            SetTorState(TorState::Unknown);
        }
    }
    else
    {
        SetError(true, "Failed to read obfs4proxy-version file.");
        SetTorState(TorState::Unknown);
    }
}

void Bridge::ReadTorProcess()
{
    std::streamoff length = 0u;
    std::ifstream file(m_BridgePath / "tor-process", std::ifstream::in);
    if (file.good())
    {
        file.seekg(0, std::ios_base::end);
        std::streamoff length = file.tellg();
        file.close();

        if (length == 0)
        {
            SetError(true, "Tor process isn't running.");
            SetTorState(TorState::Inactive);
        }
        else
        {
            SetError(false, "");
            SetTorState(TorState::Running);
        }
    }
    else
    {
        SetError(true, "Failed to read tor-process file.");
        SetTorState(TorState::Unknown);
    }
}

void Bridge::ReadTorVersion()
{

}

void Bridge::ReadBridgeStats()
{
    m_pBridgeStats->OnMonitoredDataUpdated();
}

void Bridge::ReadFingerprint()
{
    m_Fingerprint = ReadFingerprint(m_BridgePath / "fingerprint");
}

void Bridge::ReadHashedFingerprint()
{
    m_HashedFingerprint = ReadFingerprint(m_BridgePath / "hashed-fingerprint");
}

std::string Bridge::ReadFingerprint(const std::filesystem::path filePath) const
{
    std::ifstream file(filePath, std::ifstream::in);
    if (file.good())
    {
        std::string token;
        std::vector<std::string> tokens;
        while (getline(file, token, ' '))
        {
            tokens.push_back(token);
        }
        file.close();

        if (tokens.size() == 2)
        {
            std::string fingerprint = tokens[1];
            fingerprint.erase(std::remove(fingerprint.begin(), fingerprint.end(), '\n'), fingerprint.end());
            return fingerprint;
        }
    }

    return "";
}

void Bridge::ReadHeartbeatUsers()
{
    std::ifstream file(m_BridgePath / "notices.log", std::ifstream::in);
    static const std::string sHeartbeatString("Heartbeat: Since last heartbeat message");
    static const std::regex sPattern("I have seen ([0-9]+) unique clients");
    if (file.good())
    {
        std::string line;
        std::vector<std::string> tokens;

        while (getline(file, line))
        {
            if (line.find(sHeartbeatString) != std::string::npos)
            {
                std::smatch matches;
                if (std::regex_search(line, matches, sPattern)) 
                {
                    m_UniqueClients = std::stoi(matches[1]);
                }
            }
        }
        file.close();

    }
}

void Bridge::RetrieveDistributionMechanism()
{
    if (m_HashedFingerprint.empty())
    {
        return;
    }

    using json = nlohmann::json;

    std::stringstream url;
    url << "https://onionoo.torproject.org/details?lookup=" << m_HashedFingerprint;

    g_pTurbine->GetWebClient()->Get(url.str(), {},
		[this, &distributionMechanism = m_DistributionMechanism](const WebClientRequestResult& result)
		{
			json data = json::parse(result.GetData());
            json::const_iterator it = data.find("bridges");
            if (it == data.end() || !it->is_array())
            {
                return;
            }

            const json& bridges = *it;
            for (auto& bridge : bridges)
            {
                it = bridge.find("nickname");
                if (it == bridge.end() || !it->is_string() || it->get<std::string>() != this->GetName())
                {
                    continue;
                }

                it = bridge.find("bridgedb_distributor");
                if (it == bridge.end() || !it->is_string())
                {
                    continue;
                }

                distributionMechanism = it->get<std::string>();
            }
		}
	);
}

Bridge::VPSState Bridge::GetVPSState() const
{
    return m_VPSState;
}

void Bridge::SetVPSState(VPSState state)
{
    m_VPSState = state;
}

Bridge::DeploymentState Bridge::GetDeploymentState() const 
{
    return m_DeploymentState;
}

void Bridge::SetDeploymentState(DeploymentState state)
{
    m_DeploymentState = state;
}

Bridge::TorState Bridge::GetTorState() const
{
    return m_TorState;
}

void Bridge::SetTorState(TorState state)
{
    m_TorState = state;
}

std::string Bridge::GetStateText() const
{
    static std::array<std::string, static_cast<size_t>(VPSState::Count)> sVPSStateText =
    {
        "VPS active",
        "New VPS",
        "Offline",
        "Unknown VPS state"
    };

    static std::array<std::string, static_cast<size_t>(DeploymentState::Count)> sDeploymentStateText =
    {
        "Unknown",
        "Deployment pending",
        "Deploying",
        "Deployed",
        "Deployment failed",
        "Unreachable"
    };

    static std::array<std::string, static_cast<size_t>(TorState::Count)> sTorStateText =
    {
        "Unknown",
        "Unreachable",
        "Inactive",
        "Running"
    };

    if (GetVPSState() != VPSState::Active)
    {
        return sVPSStateText[static_cast<size_t>(GetVPSState())];
    }
    else if (GetDeploymentState() != DeploymentState::Deployed)
    {
        return sDeploymentStateText[static_cast<size_t>(GetDeploymentState())];
    }
    else
    {
        return sTorStateText[static_cast<size_t>(GetTorState())];
    }
}

void Bridge::SetError(bool errorState, const std::string& detail)
{
    m_Error = errorState;
    m_ErrorDetail = detail;
}

bool Bridge::GetError() const
{
    return m_Error;
}

const std::string& Bridge::GetErrorDetail() const
{
    return m_ErrorDetail;
}

std::optional<int> Bridge::GetUniqueClients() const
{
    return m_UniqueClients;
}

} // namespace Turbine