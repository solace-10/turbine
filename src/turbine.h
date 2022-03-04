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

#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include <SDL.h>

#include "camera.h"
#include "geolocationdata.h"

#include "json.h"
using json = nlohmann::json;

struct SDL_Window;

namespace Turbine
{

class HTTPClient;
class Settings;
class Provider;
class Task;
class TurbineRep;

using HTTPClientUniquePtr = std::unique_ptr<HTTPClient>;
using SettingsUniquePtr = std::unique_ptr<Settings>;
using ProviderUniquePtr = std::unique_ptr<Provider>;
using TurbineRepUniquePtr = std::unique_ptr<TurbineRep>;
using GeolocationDataMap = std::unordered_map<std::string, GeolocationDataSharedPtr>;
using TaskUniquePtr = std::unique_ptr<Task>;
using TaskVector = std::vector<TaskUniquePtr>;

class Turbine
{
public:
	Turbine(SDL_Window* pWindow, unsigned int scannerCount);
	~Turbine();
	void ProcessEvent(const SDL_Event& event);
	void Update();
    bool IsSearching() const;
    void SetSearching(bool state);
	bool IsActive() const;
	HTTPClient* GetHTTPClient();
	Settings* GetSettings() const;
    const TaskVector& GetTasks() const;
	Task* GetTask(const std::string& name) const;

	void OnMessageReceived(const json& message);

	CameraVector GetCameras() const;

private:
	void InitialiseGeolocation();
	void InitialiseCameras();
	void InitialiseProviders();
    void InitialiseTasks();
	void AddGeolocationData(const json& message);
	CameraSharedPtr FindCamera(const std::string& url);
	void ChangeCameraState(CameraSharedPtr pCamera, Camera::State state);

    bool m_Searching;
	bool m_Active;

	std::mutex m_GeolocationDataMutex;
	GeolocationDataMap m_GeolocationData;

	mutable std::mutex m_CamerasMutex;
	CameraVector m_Cameras;

	HTTPClientUniquePtr m_pHTTPClient;
	TurbineRepUniquePtr m_pRep;
	SettingsUniquePtr m_pConfiguration;
    TaskVector m_Tasks;
	std::vector<ProviderUniquePtr> m_Providers;
};

extern Turbine* g_pTurbine;

inline bool Turbine::IsSearching() const
{
    return m_Searching;
}

inline bool Turbine::IsActive() const
{
	return m_Active;
}

inline HTTPClient* Turbine::GetHTTPClient()
{
	return m_pHTTPClient.get();
}

inline Settings* Turbine::GetSettings() const
{
	return m_pConfiguration.get();
}

inline const TaskVector& Turbine::GetTasks() const
{
    return m_Tasks;
}

inline CameraVector Turbine::GetCameras() const
{
	std::scoped_lock lock(m_CamerasMutex);
	return m_Cameras;
}

} // namespace Turbine
