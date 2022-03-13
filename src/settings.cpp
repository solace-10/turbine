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

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <shlobj.h>
#undef WIN32_LEAN_AND_MEAN
#else
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#endif

#include <fstream>
#include <string>
#include <SDL.h>
#include "ext/json.hpp"
#include "settings.h"

namespace Turbine
{

Settings::Settings() :
m_DigitalOceanDropletSize("s-1vcpu-1gb"),
m_DigitalOceanDropletImage("debian-11-x64")
{
	CreateStorage();
	Load();
}

Settings::~Settings()
{
	Save();
}

void Settings::CreateStorage()
{
#ifdef _WIN32
	// Return %USERPROFILE%\Saved Games for Windows Vista or newer
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762188%28v=vs.85%29.aspx
	PWSTR pKnownFolderPath = nullptr;
	HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pKnownFolderPath);
	SDL_assert_release(result == S_OK);
	std::wstring folder(pKnownFolderPath);
	CoTaskMemFree(pKnownFolderPath);
	m_StoragePath = folder;
	m_StoragePath.append("turbine");
	std::filesystem::create_directories(m_StoragePath);
#else
	struct passwd* pw = getpwuid(getuid());
	const char* pHomeDir = pw->pw_dir;
	m_StoragePath = std::filesystem::path(pHomeDir) / ".local" / "share" / "turbine";
#endif
}

void Settings::Save()
{
	using json = nlohmann::json;
	json settings = {
		{ "providers", {
			{ "digital_ocean", {
				{"api_key", m_DigitalOceanAPIKey },
				{"droplet_size", m_DigitalOceanDropletSize },
				{"droplet_image", m_DigitalOceanDropletImage },
				{"ssh_fingerprints", m_DigitalOceanSSHFingerprints }
			}}
		}}
	};

	std::filesystem::path filePath = m_StoragePath / "settings.json";
	std::ofstream file(filePath);
	file << settings;
	file.close();
}

void Settings::Load()
{
	using json = nlohmann::json;
	std::filesystem::path filePath = m_StoragePath / "settings.json";
	std::ifstream file(filePath);
	if (file.is_open())
	{
		json settings;
		file >> settings;
		file.close();

		const json& providers = settings["providers"];
		if (providers.is_object())
		{
			const json& digitalOcean = providers["digital_ocean"];
			if (digitalOcean.is_object())
			{
				json::const_iterator it = digitalOcean.find("api_key");
				if (it != digitalOcean.end() && it->is_string())
				{
					m_DigitalOceanAPIKey = it->get<std::string>();
				}

				it = digitalOcean.find("droplet_size");
				if (it != digitalOcean.end() && it->is_string())
				{
					m_DigitalOceanDropletSize = it->get<std::string>();
				}

				it = digitalOcean.find("droplet_image");
				if (it != digitalOcean.end() && it->is_string())
				{
					m_DigitalOceanDropletImage = it->get<std::string>();
				}

				it = digitalOcean.find("ssh_fingerprints");
				if (it != digitalOcean.end() && it->is_array())
				{
					m_DigitalOceanSSHFingerprints = it->get<std::vector<std::string>>();
				}
			}
		}
	}
}

} // namespace Turbine
