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

#include "geolocation/geolocationservice.hpp"

#include "ext/json.hpp"

#include <iostream>
#include <sstream>
#include <stdio.h>

#include <curl/curl.h>
#include <imgui/imgui.h>


// This needs to be a static function as libcurl is a C library and will segfault if passed
// a local lambda.
static size_t WriteMemoryCallback( void* pContents, size_t size, size_t nmemb, void* pUserData )
{
	size_t realSize = size * nmemb;
	std::string& data = *reinterpret_cast< std::string* >( pUserData );
	size_t curDataSize = data.size();
	data.resize( curDataSize + realSize );
	memcpy( &data[ curDataSize ], pContents, realSize );
	return realSize;
}

namespace Turbine
{

GeolocationService::GeolocationService()
: m_QueryThreadActive(false)
, m_RateLimitExceeded(false)
{

}

GeolocationService::~GeolocationService()
{
	if ( m_QueryThread.joinable() )
	{
		m_QueryThread.join();
	}
}

void GeolocationService::Update(float delta)
{
	ConsumeQueue();
}

void GeolocationService::Query(const std::string& ip, GeolocationQueryResultCallback callback)
{
	std::lock_guard<std::mutex> lock(m_AccessMutex);
	QueueEntry entry;
	entry.ip = ip;
	entry.callback = callback;
	m_Queue.push_back(entry);
}

// void GeolocationService::DrawUI( ImGuiContext* pContext )
// {
// 	ImGui::SetCurrentContext( pContext );

// 	if ( ImGui::CollapsingHeader( "Geolocation", ImGuiTreeNodeFlags_DefaultOpen ) )
// 	{
// 		std::lock_guard< std::mutex > lock( m_AccessMutex );
// 		ImGui::Text( "Provider: ipinfo.io" );
		
// 		if ( m_RateLimitExceeded )
// 		{
// 			ImGui::Text( "Rate limit exceeded." );
// 		}
// 		else
// 		{
// 			std::stringstream ss;
// 			ss << "Queue size: " << m_Queue.size();
// 			ImGui::Text( ss.str().c_str() );
// 		}
// 	}
// }

// If required, create a thread which will keep consuming the queue until it is
// empty. The thread is responsible for resolving the IP addresses into actual
// locations and for sending back the "geolocation_result" message.
void GeolocationService::ConsumeQueue()
{
    using json = nlohmann::json;

	if ( m_Queue.empty() || m_QueryThreadActive )
	{
		return;
	}

	auto threadMain = []( GeolocationService* pGeolocationService )
	{
		pGeolocationService->m_QueryThreadActive = true;
		while ( 1 )
		{
			QueueEntry entry;
			{
				std::lock_guard< std::mutex > lock( pGeolocationService->m_AccessMutex );
				if ( pGeolocationService->m_Queue.empty() )
				{
					break;
				}
				else
				{
					entry = pGeolocationService->m_Queue.back();
					pGeolocationService->m_Queue.pop_back();
				}
			}

			pGeolocationService->m_Data.clear();

			std::stringstream url;
			url << "https://ipinfo.io/" << entry.ip << "/json"; 

			CURL* pCurlHandle = curl_easy_init();
			char pErrorBuffer[ CURL_ERROR_SIZE ];
			curl_easy_setopt( pCurlHandle, CURLOPT_ERRORBUFFER, pErrorBuffer );
			curl_easy_setopt( pCurlHandle, CURLOPT_URL, url.str().c_str() );
			curl_easy_setopt( pCurlHandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback );
			curl_easy_setopt( pCurlHandle, CURLOPT_WRITEDATA, &pGeolocationService->m_Data );
			curl_easy_setopt( pCurlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0" );
			curl_easy_setopt( pCurlHandle, CURLOPT_TIMEOUT, 10L );

			GeolocationQueryResult result;
			if ( curl_easy_perform( pCurlHandle ) != CURLE_OK )
			{
				result.Error = pErrorBuffer;
			}
			else
			{
				json message;
				if ( pGeolocationService->m_Data.find( "Rate limit exceeded." ) != std::string::npos )
				{
					result.Error = "Rate limit exceeded";
					pGeolocationService->m_RateLimitExceeded = true;
				}
				else
				{
					json data = json::parse( pGeolocationService->m_Data );
					if ( data.find( "city" ) != data.end() && 
						 data.find( "region" ) != data.end() &&
						 data.find( "country" ) != data.end() &&
						 data.find( "org" ) != data.end() &&
						 data.find( "loc" ) != data.end() )
					{
						float latitude = 0.0f;
						float longitude = 0.0f;
						std::string location = data["loc"];
						size_t locationSeparator = location.find_first_of(',');
						if (locationSeparator != std::string::npos)
						{
							latitude = static_cast<float>(atof(location.substr(0, locationSeparator).c_str()));
							longitude = static_cast<float>(atof(location.substr(locationSeparator + 1).c_str()));
						}

						result.Data = GeolocationData(
							entry.ip,
							data["city"],
							data["region"],
							data["country"],
							data["org"],
							latitude,
							longitude
						);
					}
					else
					{
						result.Error = "Error processing JSON response.";
					}
				}

                entry.callback(result);
			}

			curl_easy_cleanup( pCurlHandle );
		}
		pGeolocationService->m_QueryThreadActive = false;
	};

	// Ensure any previous thread is fully terminated before starting a new one.
	if ( m_QueryThread.joinable() )
	{
		m_QueryThread.join();
	}

	m_QueryThread = std::thread( threadMain, this );
}

} // namespace Turbine
