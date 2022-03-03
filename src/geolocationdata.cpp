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

#include "database/database.h"
#include "json.h"
#include "geolocationdata.h"

namespace Turbine
{

GeolocationData::GeolocationData(IPAddress address) :
	m_Address(address),
	m_Latitude(0.0f),
	m_Longitude(0.0f)
{
}

bool GeolocationData::LoadFromDatabase(const std::string& city, const std::string& region, const std::string& country, const std::string& organisation, float latitude, float longitude)
{
	m_City = city;
	m_Region = region;
	m_Country = country;
	m_Organisation = organisation;
	m_Latitude = latitude;
	m_Longitude = longitude;
	return true;
}

bool GeolocationData::LoadFromJSON(const json& geolocationDataJson)
{
	m_City = geolocationDataJson["city"].get<std::string>();
	m_Region = geolocationDataJson["region"].get<std::string>();
	m_Country = geolocationDataJson["country"].get<std::string>();
	m_Organisation = geolocationDataJson["org"].get<std::string>();

	std::string location = geolocationDataJson["loc"];
	size_t locationSeparator = location.find_first_of(',');
	if (locationSeparator == std::string::npos)
	{
		return false;
	}
	else
	{
		std::string latitude = location.substr(0, locationSeparator);
		std::string longitude = location.substr(locationSeparator + 1);
		m_Latitude = static_cast<float>(atof(latitude.c_str()));
		m_Longitude = static_cast<float>(atof(longitude.c_str()));
		return true;
	}
}

void GeolocationData::SaveToDatabase(Database* pDatabase)
{
	PreparedStatement addGeolocationStatement(pDatabase, "INSERT OR REPLACE INTO Geolocation VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7);");
	addGeolocationStatement.Bind(1, m_Address.ToString());
	addGeolocationStatement.Bind(2, m_City);
	addGeolocationStatement.Bind(3, m_Region);
	addGeolocationStatement.Bind(4, m_Country);
	addGeolocationStatement.Bind(5, m_Organisation);
	addGeolocationStatement.Bind(6, static_cast<double>(m_Latitude));
	addGeolocationStatement.Bind(7, static_cast<double>(m_Longitude));
	pDatabase->Execute(addGeolocationStatement);

	PreparedStatement updateCameraStatement(pDatabase, "UPDATE Cameras SET Geolocated=1 WHERE IP=?1;");
	updateCameraStatement.Bind(1, m_Address.ToString());
	pDatabase->Execute(updateCameraStatement);
}

}