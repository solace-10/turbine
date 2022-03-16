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

#include <sstream>

#include "bridge/bridge.h"
#include "providers/digitalocean/firewallmanager.hpp"
#include "turbine.h"

namespace Turbine
{

FirewallManager::FirewallManager() :
m_RefreshTimer(0.0f)
{

}
	
FirewallManager::~FirewallManager()
{

}

// When we get notified that a Bridge has been added, we do not yet know whether it
// has a firewall associated with it or not.
// We also don't know how many bridges are being added within a short amount of time,
// so we postpone the request until the next Update().
void FirewallManager::AddBridge(const BridgeSharedPtr& pBridge)
{
    Firewall firewall;
    firewall.m_pBridge = pBridge;
    firewall.m_State = State::Unknown;
    m_Firewalls.push_back(std::move(firewall));
    m_RefreshTimer = 0.0f;
}

// This should only be called if we are authenticated with Digital Ocean.
void FirewallManager::Update(float delta, const WebClient::Headers& headers)
{
    m_Headers = headers;

    if (m_RefreshTimer <= 0.0f)
    {
        RefreshFirewalls();       
        m_RefreshTimer = 60.0f;
    }

    for (auto& firewall : m_Firewalls)
    {
        if (firewall.m_State == State::Missing)
        {
            InstallFirewall(firewall);
        }
    }
}

// Send a request to Digital Ocean for all the firewalls.
void FirewallManager::RefreshFirewalls()
{
    g_pTurbine->GetWebClient()->Get("https://api.digitalocean.com/v2/firewalls?per_page=200", m_Headers,
        [this](const WebClientRequestResult& result)
        {
            json data = json::parse(result.GetData());
            json::const_iterator it = data.find("firewalls");
            if (it != data.end())
            {
                const json& firewallsResponse = *it;
                for (Firewall& firewall : this->m_Firewalls)
                {
                    BridgeSharedPtr pBridge = firewall.m_pBridge.lock();
                    if (pBridge != nullptr)
                    {
                        bool found = false;
                        size_t idx = 0;
                        const size_t numFirewallResponses = firewallsResponse.size();
                        for (; idx < numFirewallResponses; ++idx)
                        {
                            if (firewallsResponse[idx]["name"] == pBridge->GetName())
                            {
                                found = true;
                                break;
                            }
                        }

                        if (!found && firewall.m_State == State::Unknown)
                        {
                            firewall.m_State = State::Missing;
                            this->InstallFirewall(firewall);
                        }
                        else if (found)
                        {
                            const std::string status = firewallsResponse[idx]["status"].get<std::string>();
                            if (status == "succeeded")
                            {
                                // No need to show the log if it was previously installed.
                                if (firewall.m_State == State::Installing)
                                {
                                    Log::Info("Installed firewall on bridge %s.", pBridge->GetName().c_str());
                                }

                                firewall.m_State = State::Installed;
                            }
                            else if (status == "failed")
                            {
                                Log::Error("Installation failed on bridge '%s'.", pBridge->GetName().c_str());
                                firewall.m_State = State::InstallationFailed;
                            }
                        }
                    }
                }
            }
            else
            {
                if (data.find("message") != data.end())
                {
                    Log::Error("Error while refreshing Digital Ocean firewalls: %s.", data["message"].get<std::string>().c_str());
                }
                else
                {
                    Log::Error("Error while refreshing Digital Ocean firewalls: unknown error.");
                }
            }
        }
    );
}

void FirewallManager::InstallFirewall(Firewall& firewall)
{
    firewall.m_State = State::Installing;
    BridgeSharedPtr pBridge = firewall.m_pBridge.lock();
    Log::Info("Setting up firewall for %s.", pBridge->GetName().c_str());

    std::stringstream orPort, extPort;
    orPort << pBridge->GetORPort();
    extPort << pBridge->GetExtPort();

    nlohmann::ordered_json payload = json::object();
    payload["name"] = pBridge->GetName();
    payload["droplet_ids"] = { pBridge->GetId() };
    json sources;
    sources["addresses"] = { "0.0.0.0/0", "::/0" };

    json inboundRuleSSHPort = json::object();
    inboundRuleSSHPort["protocol"] = "tcp";
    inboundRuleSSHPort["ports"] = "22";
    inboundRuleSSHPort["sources"] = sources;
    
    json inboundRuleORPort = json::object();
    inboundRuleORPort["protocol"] = "tcp";
    inboundRuleORPort["ports"] = orPort.str();
    inboundRuleORPort["sources"] = sources;

    json inboundRuleExtPort = json::object();
    inboundRuleExtPort["protocol"] = "tcp";
    inboundRuleExtPort["ports"] = extPort.str();
    inboundRuleExtPort["sources"] = sources;

    payload["inbound_rules"] = { inboundRuleSSHPort, inboundRuleORPort, inboundRuleExtPort };

    json destinations;
    destinations["addresses"] = { "0.0.0.0/0", "::/0" };

    json outboundRuleTCP = json::object();
    outboundRuleTCP["protocol"] = "tcp";
    outboundRuleTCP["ports"] = "0"; // All ports.
    outboundRuleTCP["destinations"] = destinations;

    json outboundRuleUDP = json::object();
    outboundRuleUDP["protocol"] = "udp";
    outboundRuleUDP["ports"] = "0"; // All ports.
    outboundRuleUDP["destinations"] = destinations;

    payload["outbound_rules"] = { outboundRuleTCP, outboundRuleUDP };

    const std::string rawPayload = payload.dump();
	g_pTurbine->GetWebClient()->Post("https://api.digitalocean.com/v2/firewalls", m_Headers, rawPayload,
		[this](const WebClientRequestResult& result)
		{
            json data = json::parse(result.GetData());
            json::const_iterator it = data.find("id");
            if (it != data.end())
            {
                Log::Error("Error installing firewall: (%s) %s", 
                    data["id"].get<std::string>().c_str(), 
                    data["message"].get<std::string>().c_str()
                );
            }
		}
	);
}

} // namespace Turbine
