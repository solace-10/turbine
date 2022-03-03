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
#include <vector>

namespace Turbine
{

class Configuration
{
public:
	Configuration();
	~Configuration();

	std::filesystem::path GetStoragePath() const;
	const std::string& GetGoogleCSEApiKey() const;
	void SetGoogleCSEApiKey(const std::string& value);
	const std::string& GetGoogleCSEId() const;
	void SetGoogleCSEId(const std::string& value);

private:
	void CreateStorage();
	void Save();
	void Load();

	std::filesystem::path m_StoragePath;
	std::string m_GoogleCSEApiKey;
	std::string m_GoogleCSEId;
};

inline std::filesystem::path Configuration::GetStoragePath() const
{
	return m_StoragePath;
}

inline const std::string& Configuration::GetGoogleCSEApiKey() const
{
	return m_GoogleCSEApiKey;
}

inline void Configuration::SetGoogleCSEApiKey(const std::string& value)
{
	m_GoogleCSEApiKey = value;
}

inline const std::string& Configuration::GetGoogleCSEId() const
{
	return m_GoogleCSEId;
}

inline void Configuration::SetGoogleCSEId(const std::string& value)
{
	m_GoogleCSEId = value;
}

} // namespace Turbine
