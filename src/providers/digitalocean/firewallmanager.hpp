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

#include <memory>
#include <string>
#include <vector>

#include "webclient/webclient.h"

namespace Turbine
{

class Bridge;
using BridgeWeakPtr = std::weak_ptr<Bridge>;
using BridgeSharedPtr = std::shared_ptr<Bridge>;

class FirewallManager
{
public:
	FirewallManager();
	~FirewallManager();

	void AddBridge(const BridgeSharedPtr& pBridge);
	void Update(float delta, const WebClient::Headers& headers);

private:
	enum class State
	{
		Unknown, // We have a droplet, but don't know if there is a firewall associated with it or not.
		Missing, // We have a droplet with no firewall associated. The bridge won't be able to communicate. 
		Installing, // A request has been made to setup a firewall.
		Installed // A firewall is in place.
	};

	struct Firewall
	{
		BridgeWeakPtr m_pBridge;
		State m_State;
	};

	void RefreshFirewalls();
	void InstallFirewall(const Firewall& firewall);

	std::vector<Firewall> m_Firewalls;
	bool m_RefreshFirewalls;
	WebClient::Headers m_Headers;
};

} // namespace Turbine
