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

#include "bridge/bridge.h"
#include "providers/digitalocean/firewallmanager.hpp"
#include "turbine.h"

namespace Turbine
{

FirewallManager::FirewallManager() :
m_RefreshFirewalls(false)
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
    m_RefreshFirewalls = true;
}

// This should only be called if we are authenticated with Digital Ocean.
void FirewallManager::Update(float delta, const WebClient::Headers& headers)
{
    m_Headers = headers;

    if (m_RefreshFirewalls)
    {
        RefreshFirewalls();       
        m_RefreshFirewalls = false;
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
            int a = 0;
        }
    );
}

void FirewallManager::InstallFirewall(const Firewall& firewall)
{

}

} // namespace Turbine
