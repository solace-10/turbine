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

#include <sstream>

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

const std::string& DigitalOceanProvider::GetName() const
{
	return m_Name;
}

bool DigitalOceanProvider::IsAuthenticated() const
{
	return m_Authenticated;
}

void DigitalOceanProvider::Authenticate()
{

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

} // namespace Turbine
