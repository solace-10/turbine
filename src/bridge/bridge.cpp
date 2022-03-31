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

#include "bridge/bridge.h"
#include "bridge/bridgestats.hpp"
#include "bridge/bridgesummarywidget.h"
#include "bridge/statemachine.h"
#include "deployment/deployment.h"
#include "webclient/webclient.h"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

Bridge::Bridge(Provider* pProvider, const std::string& id, const std::string& name, const std::string& initialState) :
m_pProvider(pProvider),
m_Id(id),
m_Name(name),
m_ORPort(0),
m_ExtPort(0),
m_DistributionMechanism("unknown")
{
    m_pBridgeSummaryWidget = std::make_unique<BridgeSummaryWidget>();
    m_pStateMachine = std::make_unique<StateMachine>(name);
    InitialiseStateMachine();
    m_pStateMachine->SetState(initialState, true);

    m_BridgePath = g_pTurbine->GetSettings()->GetStoragePath() / "bridges" / GetName();
    m_pBridgeStats = std::make_shared<BridgeStats>(this);

    OnMonitoredDataUpdated();
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

const std::string& Bridge::GetState() const
{
    return m_pStateMachine->GetState();
}
    
void Bridge::SetState(const std::string& state, bool force /* = false */)
{
    m_pStateMachine->SetState(state, force);
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
}

void Bridge::SetIPv6(const std::string& ip)
{
    m_Ipv6 = ip;
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

void Bridge::RenderSummaryWidget()
{
    m_pBridgeSummaryWidget->Render(this);
}

void Bridge::InitialiseStateMachine()
{
    m_pStateMachine->AddState("Offline");
    m_pStateMachine->AddState("Shutting down");
    m_pStateMachine->AddState("New");
    m_pStateMachine->AddState("Deployment pending");
    m_pStateMachine->AddState("Deploying");
    m_pStateMachine->AddState("Deployed");
    m_pStateMachine->AddState("Deployment failed");

    m_pStateMachine->LinkStates("Offline", "New");

    m_pStateMachine->LinkStates("Shutting down", "Offline");

	m_pStateMachine->LinkStates("New", "Shutting down");
	m_pStateMachine->LinkStates("New", "Offline");
    m_pStateMachine->LinkStates("New", "Deployment needed");
    m_pStateMachine->LinkStates("New", "Deploying");
}

void Bridge::OnMonitoredDataUpdated()
{
    ReadBridgeStats();
    ReadFingerprint();
    ReadHashedFingerprint();
    RetrieveDistributionMechanism();
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

void Bridge::RetrieveDistributionMechanism()
{
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

} // namespace Turbine