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

#include <filesystem>
#include <fstream>
#include <unordered_map>

#include "bridge/bridge.h"
#include "bridge/bridgegeolocation.hpp"
#include "webclient/webclient.h"
#include "json.hpp"
#include "log.h"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

BridgeGeolocation::BridgeGeolocation(const std::string& address) :
m_IsValid(false),
m_Address(address),
m_Latitude(0.0f),
m_Longitude(0.0f)
{
    m_Hash = CreateHash(address);

    static bool sDirectoriesCreated = false;
    if (sDirectoriesCreated == false)
    {
        std::filesystem::path filePath = g_pTurbine->GetSettings()->GetStoragePath() / "geolocation";
        std::filesystem::create_directories(filePath);
        sDirectoriesCreated = true;
    }

    if (LoadFromFile() == false)
    {
        RequestGeolocation();
    }
}

std::string BridgeGeolocation::CreateHash(const std::string& value) const
{
    std::stringstream ss;
    ss << std::hash<std::string>{}(value);
    return ss.str();
}

bool BridgeGeolocation::LoadFromFile()
{
    using namespace nlohmann;

    std::filesystem::path filePath = g_pTurbine->GetSettings()->GetStoragePath() / "geolocation" / this->m_Hash;
    std::ifstream file(filePath);
    if (file.is_open())
    {
        json data = json::parse(file);

        auto ReadString = [&data](const std::string& source, std::string& out) -> bool {
            json::iterator it = data.find(source);
            if (it != data.end() && it->is_string())
            {
                out = it->get<std::string>();
                return true;
            }
            else
            {
                return false;
            }
        };
        
        ReadString("ip", m_Address);
        ReadString("city", m_City);
        ReadString("region", m_Region);
        ReadString("country", m_Country);
        ReadString("org", m_Organisation);
        
        std::string location;
        if (ReadString("loc", location))
        {
            std::stringstream ss(location);
            ss >> m_Latitude;
            ss.ignore(1, ',');
            ss >> m_Longitude;

            m_IsValid = m_Latitude != 0.0f && m_Longitude != 0.0f;
        }

        file.close();
        return m_IsValid;
    }

    return false;
}

void BridgeGeolocation::RequestGeolocation()
{
    const std::string& key = g_pTurbine->GetSettings()->GetIPInfoAPIKey();
    if (key.empty())
    {
        return;
    }

    std::stringstream url;
    url << "http://ipinfo.io/" << m_Address << "?token=" << key;

	g_pTurbine->GetWebClient()->Get(url.str(), {},
		[this](const WebClientRequestResult& result)
		{
            std::filesystem::path filePath = g_pTurbine->GetSettings()->GetStoragePath() / "geolocation" / this->m_Hash;
            std::ofstream file(filePath);
            if (file.is_open())
            {
                file << result.GetData();
                file.close();
            }
            else
            {
                Log::Error("Unable to open '%s' for write.", filePath.c_str());
            }
		}
	);
}

} // namespace Turbine
