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

#include <iostream>
#include <sstream>
#include <vector>

#include "bridge/bridge.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "providers/digitalocean/digitaloceanprovider.h"
#include "providers/digitalocean/dropletinfo.h"
#include "providers/digitalocean/firewallmanager.hpp"
#include "providers/digitalocean/imageinfo.h"
#include "webclient/webclient.h"
#include "log.h"
#include "json.hpp"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

DigitalOceanProvider::DigitalOceanProvider() :
	m_Name("Digital Ocean"),
	m_Authenticated(false),
	m_AuthenticationInFlight(false),
	m_DropletMonitorTimer(0.0f)
{
	m_pFirewallManager = std::make_unique<FirewallManager>();
}

DigitalOceanProvider::~DigitalOceanProvider()
{

}

void DigitalOceanProvider::Update(float delta)
{
	if (HasAPIKeyChanged())
	{
		RebuildHeaders();
		m_Authenticated = false;
	}

	if (!IsAuthenticated())
	{
		TryAuthenticate();
	}
	else
	{
		UpdateDropletMonitor(delta);
		m_pFirewallManager->Update(delta, m_Headers);
	}
}

void DigitalOceanProvider::RenderSettings()
{
	static std::string apiKey = g_pTurbine->GetSettings()->GetDigitalOceanAPIKey();

	ImGui::Text("Status:");
	ImGui::SameLine();
	if (IsAuthenticated())
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Authenticated");
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not authenticated");
	}

	if (ImGui::InputText("Personal Access Token", &apiKey))
	{
		g_pTurbine->GetSettings()->SetDigitalOceanAPIKey(apiKey);
	}
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted("In order to use Digital Ocean with Turbine, you need to provide a Personal Access Token. The token must have Read and Write scopes.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	const std::string& dropletSize = g_pTurbine->GetSettings()->GetDigitalOceanDropletSize();
	if (ImGui::BeginCombo("Droplet size", dropletSize.c_str(), 0))
	{
		for (auto const& dropletInfoPair : m_DropletInfoMap)
		{
			const DropletInfo* pDropletInfo = dropletInfoPair.second.get();
			const bool is_selected = dropletSize == pDropletInfo->GetName();
			if (ImGui::Selectable(pDropletInfo->GetName().c_str(), is_selected))
			{
				g_pTurbine->GetSettings()->SetDigitalOceanDropletSize(pDropletInfo->GetName());
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text("vCPUs: %d", pDropletInfo->GetCPUs());
				ImGui::Text("Memory: %.2fMB", pDropletInfo->GetMemory());
				ImGui::Text("Disk: %.2fGB", pDropletInfo->GetDisk());
				ImGui::Text("Transfer: %.2fTB", pDropletInfo->GetTransfer());
				ImGui::Text("Price per month: %.2fUSD", pDropletInfo->GetPrice());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	RenderDropletImageSettings();

	static std::string fingerprints = ArrayToInputField(g_pTurbine->GetSettings()->GetDigitalOceanSSHFingerprints());
	if (ImGui::InputTextMultiline("SSH key fingerprints", &fingerprints))
	{
		g_pTurbine->GetSettings()->SetDigitalOceanSSHFingerprints(InputFieldToArray(fingerprints));
	}
}

void DigitalOceanProvider::RenderDropletImageSettings()
{
	const std::string& savedDropletImage = g_pTurbine->GetSettings()->GetDigitalOceanDropletImage();
	ImageInfo* pSelectedDropletImage = nullptr;
	for (auto const& pDropletImage : m_Images)
	{
		if (pDropletImage->GetSlug() == savedDropletImage)
		{
			pSelectedDropletImage = pDropletImage.get();
			break;
		}
	}

	if (ImGui::BeginCombo("Droplet image", pSelectedDropletImage ? pSelectedDropletImage->GetDisplayText().c_str() : "", 0))
	{
		for (auto const& pDropletImage : m_Images)
		{
			const bool is_selected = pDropletImage.get() == pSelectedDropletImage;
			if (ImGui::Selectable(pDropletImage->GetDisplayText().c_str(), is_selected))
			{
				g_pTurbine->GetSettings()->SetDigitalOceanDropletImage(pDropletImage->GetSlug());
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

const std::string& DigitalOceanProvider::GetName() const
{
	return m_Name;
}

bool DigitalOceanProvider::IsAuthenticated() const
{
	return m_Authenticated;
}

void DigitalOceanProvider::CreateBridge(const std::string& name, bool isListed)
{
	Settings* pSettings = g_pTurbine->GetSettings();
	if (pSettings->GetContactEmail().empty())
	{
		Log::Error("Can't create bridge: contact email must be set.");
		return;
	}
	else if (pSettings->GetDigitalOceanSSHFingerprints().empty())
	{
		Log::Error("Can't create bridge: no SSH keys registered.");
		return;
	}

	std::string region = GetRandomRegion(pSettings->GetDigitalOceanDropletSize());
	if (region.empty())
	{
		Log::Error("Can't create bridge: couldn't get region.");
		return;
	}

	Log::Info("Creating %s bridge '%s'...", isListed ? "listed" : "unlisted", name.c_str());
	std::string turbineTypeTag = isListed ? "turbine_listed" : "turbine_unlisted";
	std::string turbineORPortTag, turbineExtPortTag;
	CreateTorPortTags(turbineORPortTag, turbineExtPortTag);
	json payload;
	payload["name"] = name;
	payload["region"] = region;
	payload["size"] = pSettings->GetDigitalOceanDropletSize();
	payload["image"] = pSettings->GetDigitalOceanDropletImage();
	payload["ssh_keys"] = pSettings->GetDigitalOceanSSHFingerprints();
	payload["ipv6"] = true;
	payload["tags"] = { "turbine", "turbine_deployment_pending", turbineTypeTag, turbineORPortTag, turbineExtPortTag };
	payload["password"] = nullptr;

	const std::string rawPayload = payload.dump();
	g_pTurbine->GetWebClient()->Post("https://api.digitalocean.com/v2/droplets", m_Headers, rawPayload,
		[this](const WebClientRequestResult& result)
		{
			int a = 0;
		}
	);
}

std::string DigitalOceanProvider::GetRandomRegion(const std::string& dropletSize) const
{
	DropletInfoMap::const_iterator it = m_DropletInfoMap.find(dropletSize);
	if (it == m_DropletInfoMap.end())
	{
		return "";
	}
	else
	{
		const Regions& regions = it->second->GetRegions();
		if (regions.empty())
		{
			return "";
		}
		else
		{
			return regions[rand()%regions.size()];
		}
	}
}

void DigitalOceanProvider::OnBridgeDeployed(Bridge* pBridge)
{
	json payload;
	payload["resources"] = 	{{
		{ "resource_id", pBridge->GetId() },
		{ "resource_type", "droplet" }
	}};

	const std::string rawPayload = payload.dump();
	g_pTurbine->GetWebClient()->Delete("https://api.digitalocean.com/v2/tags/turbine_deployment_pending/resources", m_Headers, rawPayload,
		[this](const WebClientRequestResult& result)
		{
		}
	);
}

bool DigitalOceanProvider::HasAPIKeyChanged()
{
	const std::string& settingsKey = g_pTurbine->GetSettings()->GetDigitalOceanAPIKey();
	if (m_APIKey != settingsKey)
	{
		m_APIKey = settingsKey;
		return true;
	}
	else
	{
		return false;
	}
}

void DigitalOceanProvider::TryAuthenticate()
{
	const std::string& token = g_pTurbine->GetSettings()->GetDigitalOceanAPIKey();
	if (IsAuthenticated() == false && m_AuthenticationInFlight == false && token.empty() == false)
	{
		m_AuthenticationInFlight = true;
		g_pTurbine->GetWebClient()->Get("https://api.digitalocean.com/v2/account", m_Headers,
			[this](const WebClientRequestResult& result)
			{
				json data = json::parse(result.GetData());
				if (data.is_object() && data.find("account") != data.end())
				{
					std::string status = data["account"]["status"].get<std::string>();
					m_Authenticated = (status == "active");
					Log::Info("Authenticated with Digital Ocean.");
				}
				else
				{
					m_Authenticated = false;
				}

				this->m_AuthenticationInFlight = false;
				RebuildDropletInfoMap();
				RebuildImages();
			}
		);
	}
}

void DigitalOceanProvider::RebuildHeaders()
{
	m_Headers.clear();
	m_Headers.emplace_back("Content-Type: application/json");
	m_Headers.emplace_back("Authorization: Bearer " + m_APIKey);
}

void DigitalOceanProvider::RebuildDropletInfoMap()
{
	g_pTurbine->GetWebClient()->Get("https://api.digitalocean.com/v2/sizes?per_page=200", m_Headers,
		[this](const WebClientRequestResult& result)
		{
			json data = json::parse(result.GetData());
			if (data.is_object() && data.find("sizes") != data.end())
			{
				m_DropletInfoMap.clear();
				const json& sizes = data["sizes"];
				if (sizes.is_array())
				{
					const size_t numSizes = sizes.size();
					for (size_t i = 0; i < numSizes; ++i)
					{
						const json& entry = sizes[i];
						const std::string& name = entry["slug"].get<std::string>();
						const float memory = entry["memory"].get<float>();
						const int cpus = entry["vcpus"].get<int>();
						const float disk = entry["disk"].get<float>();
						const float transfer = entry["transfer"].get<float>();
						const float priceMonthly = entry["price_monthly"].get<float>();
						Regions availableRegions;
						const json& regions = entry["regions"];
						const size_t numRegions = regions.size();
						for (size_t j = 0; j < numRegions; ++j)
						{
							availableRegions.push_back(regions[j].get<std::string>());
						}
						m_DropletInfoMap[name] = std::make_unique<DropletInfo>(name, memory, cpus, disk, transfer, priceMonthly, availableRegions);
					}
				}
			}
		}
	);
}

void DigitalOceanProvider::RebuildImages()
{
	g_pTurbine->GetWebClient()->Get("https://api.digitalocean.com/v2/images?type=distribution&per_page=200", m_Headers,
		[this](const WebClientRequestResult& result)
		{
			json data = json::parse(result.GetData());
			if (data.is_object() && data.find("images") != data.end())
			{
				const json& images = data["images"];
				if (images.is_array())
				{
					m_Images.clear();
					size_t numImages = images.size();
					for (size_t i = 0; i < numImages; ++i)
					{
						const json& image = images[i];
						m_Images.emplace_back(
							std::make_unique<ImageInfo>(
								image["name"].get<std::string>(),
								image["distribution"].get<std::string>(),
								image["slug"].get<std::string>()
								)
						);
					}
				}
			}
		}
	);
}

void DigitalOceanProvider::UpdateDropletMonitor(float delta)
{
	m_DropletMonitorTimer -= delta;
	if (m_DropletMonitorTimer <= 0.0f)
	{
		m_DropletMonitorTimer = 15.0f;

		g_pTurbine->GetWebClient()->Get("https://api.digitalocean.com/v2/droplets?per_page=200", m_Headers,
			[this](const WebClientRequestResult& result)
			{
				json data = json::parse(result.GetData());
				if (data.is_object() && data.find("droplets") != data.end())
				{
					json::const_iterator it = data.find("droplets");
					if (it != data.end() && it->is_array())
					{
						size_t numDroplets = it->size();
						for (auto& droplet : *it)
						{
							int rawId = droplet["id"].get<int>();
							std::stringstream idss;
							idss << rawId;
							const std::string& id = idss.str();
							const std::string& name = droplet["name"].get<std::string>();
							const std::string& dropletState = droplet["status"].get<std::string>();
							std::vector<std::string> tags = droplet["tags"];

							if (IsTurbineDroplet(tags) == false)
							{
								continue;
							}

							const std::string& ipv4 = ExtractIP(droplet, "v4");
							const std::string& ipv6 = ExtractIP(droplet, "v6");
							
							Bridge* pExistingBridge = g_pTurbine->GetBridge(id);
							Bridge::VPSState vpsState = Bridge::VPSState::Unknown;
							Bridge::DeploymentState deploymentState = Bridge::DeploymentState::Unknown;
							GetBridgeState(dropletState, tags, vpsState, deploymentState);
							if (pExistingBridge)
							{
								pExistingBridge->SetVPSState(vpsState);
								pExistingBridge->SetIPv4(ipv4);
								pExistingBridge->SetIPv6(ipv6);
							}
							else
							{
								BridgeSharedPtr pBridge = std::make_shared<Bridge>(this, id, name, vpsState, deploymentState, Bridge::TorState::Unknown);
								pBridge->SetIPv4(ipv4);
								pBridge->SetIPv6(ipv6);
								pBridge->SetORPort(this->ExtractORPort(tags));
								pBridge->SetExtPort(this->ExtractExtPort(tags));
								m_pFirewallManager->AddBridge(pBridge);
								g_pTurbine->AddBridge(std::move(pBridge));
							}
						}
					}
				}
			}
		);
	}
}

std::string DigitalOceanProvider::ExtractIP(const nlohmann::json& droplet, const std::string& ipVersion) const
{
	json::const_iterator it = droplet.find("networks");
	if (it != droplet.end())
	{
		const json& networks = *it;
		it = networks.find(ipVersion);
		if (it != networks.end())
		{
			const json& version = *it;
			if (version.is_array() && version.size() > 0)
			{
				const size_t numAddresses = version.size();
				for (size_t i = 0; i < numAddresses; ++i)
				{
					const json& address = version[i];
					it = address.find("type");
					if (it == address.end() || it->is_string() == false || it->get<std::string>() != "public")
					{
						continue;
					}

					it = address.find("ip_address");
					if (it != address.end())
					{
						return it->get<std::string>();
					}
				}
			}
		}
	}	
	return "";
}

void DigitalOceanProvider::GetBridgeState(const std::string& dropletState, const std::vector<std::string>& tags, Bridge::VPSState& vpsState, Bridge::DeploymentState& deploymentState) const
{
	if (dropletState == "active")
	{
		vpsState = Bridge::VPSState::Active;
		static const std::string sDeploymentPendingState("Deployment pending");
		static const std::string sDeployedState("Deployed");
		const bool deploymentPending = std::find(tags.begin(), tags.end(), "turbine_deployment_pending") != tags.end();
		deploymentState = deploymentPending ? Bridge::DeploymentState::DeploymentPending : Bridge::DeploymentState::Deployed; 
	}
	else if (dropletState == "new")
	{
		vpsState = Bridge::VPSState::New;
		deploymentState = Bridge::DeploymentState::Unknown;
	}
	else if (dropletState == "off")
	{
		vpsState = Bridge::VPSState::Offline;
		deploymentState = Bridge::DeploymentState::Unknown;
	}
	else
	{
		vpsState = Bridge::VPSState::Unknown;
		deploymentState = Bridge::DeploymentState::Unknown;
	}
}

std::string DigitalOceanProvider::ArrayToInputField(const std::vector<std::string>& value) const
{
	std::stringstream ss;
	for (const std::string& v : value)
	{
		ss << v << "\n";
	}
	return ss.str();
}

std::vector<std::string> DigitalOceanProvider::InputFieldToArray(const std::string& value) const
{
    const char separator = '\n';
    std::vector<std::string> v;
    std::stringstream streamData(value);
    std::string s;
    while (std::getline(streamData, s, separator)) 
	{
        v.push_back(s);
    }
	return v;
}

void DigitalOceanProvider::CreateTorPortTags(std::string& orPortTag, std::string& extPortTag) const
{
	int orPort, extPort;
	GetRandomTorPorts(orPort, extPort);

	std::stringstream orPortTagStream;
	orPortTagStream << "turbine_orport_" << orPort;
	orPortTag = orPortTagStream.str();

	std::stringstream extPortTagStream;
	extPortTagStream << "turbine_extport_" << extPort;
	extPortTag = extPortTagStream.str();
}

unsigned int DigitalOceanProvider::ExtractORPort(const std::vector<std::string>& tags) const
{
	return ExtractPort(tags, "turbine_orport_");
}

unsigned int DigitalOceanProvider::ExtractExtPort(const std::vector<std::string>& tags) const
{
	return ExtractPort(tags, "turbine_extport_");
}

unsigned int DigitalOceanProvider::ExtractPort(const std::vector<std::string>& tags, const std::string& beginsWith) const
{
	unsigned int port = 0;
	for (const std::string& tag : tags)
	{
		if (tag.rfind(beginsWith, 0) == 0)
		{
			std::stringstream ss(tag.substr(beginsWith.size()));
			ss >> port;
			break;
		}
	}
	return port;
}

bool DigitalOceanProvider::IsTurbineDroplet(const std::vector<std::string>& tags) const
{
	for (const std::string& tag : tags)
	{
		if (tag == "turbine")
		{
			return true;
		}
	}
	return false;
}

} // namespace Turbine
