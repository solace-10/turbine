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

#include "geolocation/geolocationqueryresult.hpp"

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using CURL = void;

namespace Turbine
{

using GeolocationQueryResultCallback = std::function<void(const GeolocationQueryResult&)>;

class GeolocationService
{
public:
	GeolocationService();
	~GeolocationService();

	void Update(float delta);

    void Query(const std::string& ip, GeolocationQueryResultCallback callback);

private:
	struct QueueEntry
	{
		std::string ip;
		GeolocationQueryResultCallback callback;
	};

	void ConsumeQueue();

	std::mutex m_AccessMutex;
	std::vector<QueueEntry> m_Queue;
	std::thread m_QueryThread;
	std::atomic_bool m_QueryThreadActive;
	std::string m_Data;
	bool m_RateLimitExceeded;
};

} // namespace Turbine
