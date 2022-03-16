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

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ext/json.hpp"
#include "bridge/bridge.h"
#include "providers/provider.h"
#include "webclient/webclient.h"

namespace Turbine
{

class DigitalOceanProvider;
using DigitalOceanProviderUniquePtr = std::unique_ptr<DigitalOceanProvider>;
class DropletInfo;
class ImageInfo;
class FirewallManager;
using FirewallManagerUniquePtr = std::unique_ptr<FirewallManager>;

class DigitalOceanProvider : public Provider
{
public:
	DigitalOceanProvider();
	virtual ~DigitalOceanProvider() override;

	virtual void Update(float delta) override;
	virtual void RenderSettings() override;

	virtual const std::string& GetName() const override;
	virtual bool IsAuthenticated() const override;
	virtual void CreateBridge(const std::string& name, bool isListed) override;

private:
	bool HasAPIKeyChanged();
	void TryAuthenticate();
	void RebuildHeaders();
	void RebuildDropletInfoMap();
	void RebuildImages();
	void RenderDropletImageSettings();
	void UpdateDropletMonitor(float delta);
	bool ShouldChangeBridgeState(const std::string& dropletState, const std::string& currentBridgeState) const;
	const std::string& GetBridgeState(const std::string& value, const std::vector<std::string>& tags) const;
	std::string ArrayToInputField(const std::vector<std::string>& value) const;
	std::vector<std::string> InputFieldToArray(const std::string& value) const;
	std::string ExtractIP(const nlohmann::json& droplet, const std::string& ipVersion) const;
	void CreateTorPortTags(std::string& orPort, std::string& extPort) const;
	unsigned int ExtractORPort(const std::vector<std::string>& tags) const;
	unsigned int ExtractExtPort(const std::vector<std::string>& tags) const;
	unsigned int ExtractPort(const std::vector<std::string>& tags, const std::string& beginsWith) const;
	bool IsTurbineDroplet(const std::vector<std::string>& tags) const;

	std::string m_Name;
	std::string m_APIKey;
	bool m_Authenticated;
	bool m_AuthenticationInFlight;
	WebClient::Headers m_Headers;

	using DropletInfoMap = std::map<std::string, std::unique_ptr<DropletInfo>>;
	DropletInfoMap m_DropletInfoMap;

	using ImagesVector = std::vector<std::unique_ptr<ImageInfo>>;
	ImagesVector m_Images;

	float m_DropletMonitorTimer;
	FirewallManagerUniquePtr m_pFirewallManager;
};

} // namespace Turbine
