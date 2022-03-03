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
#include "ext/json.h"
#include "configuration.h"

namespace Turbine
{

static const std::string sGoogleCSEApiKey("google_cse_api_key");
static const std::string sGoogleCSEId("google_cse_id");

Configuration::Configuration() :
m_GoogleCSEApiKey("AIzaSyBHnMEOSkTM7Lazvou27FXgJb6M4hjn9uE"),
m_GoogleCSEId("016794710981670214596:sgntarey42m")
{
	CreateStorage();
	Load();
}

Configuration::~Configuration()
{
	Save();
}

void Configuration::CreateStorage()
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

void Configuration::Save()
{
	using json = nlohmann::json;
	json config;
	config[sGoogleCSEApiKey] = m_GoogleCSEApiKey;
	config[sGoogleCSEId] = m_GoogleCSEId;

	std::filesystem::path filePath = m_StoragePath / "config.json";
	std::ofstream file( filePath );
	file << config;
	file.close();
}

void Configuration::Load()
{
	using json = nlohmann::json;
	std::ifstream file( "config.json" );
	if ( file.is_open() )
	{
		json config;
		file >> config;
		file.close();

		json jGoogleCSEApiKey = config[sGoogleCSEApiKey];
		if (jGoogleCSEApiKey.is_string())
		{
			m_GoogleCSEApiKey = jGoogleCSEApiKey.get<std::string>();
		}

		json jGoogleCSEId = config[sGoogleCSEId];
		if (jGoogleCSEId.is_string())
		{
			m_GoogleCSEId = jGoogleCSEId.get<std::string>();
		}
	}
}

} // namespace Turbine
