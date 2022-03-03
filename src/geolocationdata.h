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

#include <memory>
#include "ipaddress.h"
#include "json.h"
using json = nlohmann::json;

namespace Turbine
{

class Database;
class GeolocationData;
using GeolocationDataSharedPtr = std::shared_ptr<GeolocationData>;

class GeolocationData
{
public:
	GeolocationData(IPAddress address);
	bool LoadFromDatabase(const std::string& city, const std::string& region, const std::string& country, const std::string& organisation, float x, float y);
	bool LoadFromJSON(const json& message);
	void SaveToDatabase(Database* pDatabase);

	const IPAddress& GetIPAddress() const;
	const std::string& GetCity() const;
	const std::string& GetRegion() const;
	const std::string& GetCountry() const;
	const std::string& GetOrganisation() const;
	float GetLatitude() const;
	float GetLongitude() const;

private:
	IPAddress m_Address;
	std::string m_City;
	std::string m_Region;
	std::string m_Country;
	std::string m_Organisation;
	float m_Latitude;
	float m_Longitude;
};

inline const IPAddress& GeolocationData::GetIPAddress() const
{
	return m_Address;
}

inline const std::string& GeolocationData::GetCity() const
{
	return m_City;
}

inline const std::string& GeolocationData::GetRegion() const
{
	return m_Region;
}

inline const std::string& GeolocationData::GetCountry() const
{
	return m_Country;
}

inline const std::string& GeolocationData::GetOrganisation() const
{
	return m_Organisation;
}

inline float GeolocationData::GetLatitude() const
{
	return m_Latitude;
}

inline float GeolocationData::GetLongitude() const
{
	return m_Longitude;
}

} // namespace Turbine
