// Copyright 2023 Pedro Nunes
//
// This file is part of Turbine.
//
// Turbine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Turbine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Turbine. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include "json.hpp"
using json = nlohmann::json;

namespace Turbine
{

class GeolocationData;
using GeolocationDataSharedPtr = std::shared_ptr<GeolocationData>;

class GeolocationData
{
public:
	GeolocationData(
		const std::string& address, 
		const std::string& city, 
		const std::string& region,
		const std::string& country,
		const std::string& organisation,
		float latitude,
		float longitude);

	const std::string& GetIPAddress() const;
	const std::string& GetCity() const;
	const std::string& GetRegion() const;
	const std::string& GetCountry() const;
	const std::string& GetOrganisation() const;
	float GetLatitude() const;
	float GetLongitude() const;

private:
	std::string m_Address;
	std::string m_City;
	std::string m_Region;
	std::string m_Country;
	std::string m_Organisation;
	float m_Latitude;
	float m_Longitude;
};

inline const std::string& GeolocationData::GetIPAddress() const
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
