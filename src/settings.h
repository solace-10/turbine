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

#include <filesystem>

namespace Turbine
{

class Settings
{
public:
	Settings();
	~Settings();

	std::filesystem::path GetStoragePath() const;
	const std::string& GetDigitalOceanAPIKey() const;
	void SetDigitalOceanAPIKey(const std::string& value);
	const std::string& GetDigitalOceanDropletSize() const;
	void SetDigitalOceanDropletSize(const std::string& value);

private:
	void CreateStorage();
	void Save();
	void Load();

	std::filesystem::path m_StoragePath;
	std::string m_DigitalOceanAPIKey;
	std::string m_DigitalOceanDropletSize;
};

inline std::filesystem::path Settings::GetStoragePath() const
{
	return m_StoragePath;
}

inline const std::string& Settings::GetDigitalOceanAPIKey() const
{
	return m_DigitalOceanAPIKey;
}

inline void Settings::SetDigitalOceanAPIKey(const std::string& value)
{
	m_DigitalOceanAPIKey = value;
}

inline const std::string& Settings::GetDigitalOceanDropletSize() const
{
	return m_DigitalOceanDropletSize;
}

inline void Settings::SetDigitalOceanDropletSize(const std::string& value)
{
	m_DigitalOceanDropletSize = value;
}

} // namespace Turbine
