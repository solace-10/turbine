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

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "providers/digitalocean/digitaloceanprovider.h"
#include "webclient/webclient.h"
#include "log.h"
#include "json.h"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

DigitalOceanProvider::DigitalOceanProvider() :
	m_Name("Digital Ocean"),
	m_Authenticated(false),
	m_AuthenticationInFlight(false)
{
	
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
	Log::Info("Creating %s bridge '%s'...", isListed ? "listed" : "unlisted", name.c_str());
	std::string turbineTypeTag = isListed ? "turbine_listed" : "turbine_unlisted";
	json payload;
	payload["name"] = name;
	payload["region"] = "nyc3";
	payload["size"] = g_pTurbine->GetSettings()->GetDigitalOceanDropletSize();
	payload["image"] = "ubuntu-20-04-x64";
	payload["ssh_keys"] = { "fa:50:a6:d0:48:12:41:a8:0a:c5:31:37:32:1a:ec:b5" };
	payload["ipv6"] = true;
	payload["tags"] = { "turbine", "turbine_deployment_pending", turbineTypeTag };

	std::string payloadDumped = payload.dump();

	g_pTurbine->GetWebClient()->Post("https://api.digitalocean.com/v2/droplets", m_Headers, payloadDumped,
		[this](const WebClientRequestResult& result)
		{
			int a = 0;
		}
	);
}

BridgeList DigitalOceanProvider::GetBridges() const
{
	BridgeList bridgeList;
	return bridgeList;
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

} // namespace Turbine
