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

#include "providers/digitalocean/digitaloceanprovider.h"
#include "httpclient.h"
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
	TryAuthenticate();
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

void DigitalOceanProvider::TryAuthenticate()
{
	const std::string& token = g_pTurbine->GetSettings()->GetDigitalOceanAPIKey();
	if (IsAuthenticated() == false && m_AuthenticationInFlight == false && token.empty() == false)
	{
		m_AuthenticationInFlight = true;

		g_pTurbine->GetHTTPClient()->Request("https://api.digitalocean.com/v2/account",
			[this](const HTTPRequestResult& result)
			{
				int a = 0;

				this->m_AuthenticationInFlight = false;
			}
		);
	}
}

} // namespace Turbine
