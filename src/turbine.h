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

#include "bridge/bridge.h"
#include "geolocation/geolocationdata.hpp"
#include "json.hpp"
#include "log.h"

using json = nlohmann::json;

struct SDL_Window;

namespace Turbine
{

class Bridge;
class Deployment;
class GeolocationService;
class Monitor;
class Settings;
class Provider;
class TurbineRep;
class VeilidNetwork;
class WebClient;
class Window;

using BridgeList = std::list<Bridge*>;
using DeploymentUniquePtr = std::unique_ptr<Deployment>;
using GeolocationServiceUniquePtr = std::unique_ptr<GeolocationService>;
using MonitorUniquePtr = std::unique_ptr<Monitor>;
using SettingsUniquePtr = std::unique_ptr<Settings>;
using ProviderUniquePtr = std::unique_ptr<Provider>;
using TurbineRepUniquePtr = std::unique_ptr<TurbineRep>;
using ProviderVector = std::vector<ProviderUniquePtr>;
using VeilidNetworkUniquePtr = std::unique_ptr<VeilidNetwork>;
using WebClientUniquePtr = std::unique_ptr<WebClient>;
using WindowUniquePtr = std::unique_ptr<Window>;

class Turbine
{
public:
	Turbine(SDL_Window* pWindow, unsigned int scannerCount);
	~Turbine();
	void ProcessEvent(const SDL_Event& event);
	void Update();
	bool IsActive() const;
	Settings* GetSettings() const;
	WebClient* GetWebClient();
	Window* GetBridgesWindow();
	Window* GetCreateBridgeWindow();
	Window* GetLogWindow();
	Window* GetOverviewWindow();
	Window* GetSettingsWindow();
	Window* GetBridgeWindow(Bridge* pBridge);

	const std::string& GetLatestTorVersion() const;
	const std::string& GetLatestObfs4ProxyVersion() const;

	ProviderVector& GetProviders();
	void AddBridge(BridgeSharedPtr&& bridge);
	Bridge* GetBridge(const std::string& id);
	const Bridge* GetBridge(const std::string& id) const;
	BridgeList GetBridges() const;
	Deployment* GetDeployment() const;
	Monitor* GetMonitor() const;
	GeolocationService* GetGeolocationService() const;

private:
	void InitialiseLatestVersions();
	void InitialiseLatestTorVersion();
	void InitialiseLatestObfs4ProxyVersion();
	void InitialiseLoggers(SDL_Window* pWindow);
	void InitialiseProviders();

	bool m_Active;

	WebClientUniquePtr m_pWebClient;
	TurbineRepUniquePtr m_pRep;
	SettingsUniquePtr m_pConfiguration;
	std::vector<ProviderUniquePtr> m_Providers;
	std::shared_ptr<NotificationLogger> m_pNotificationLogger;
	WindowUniquePtr m_pBridgesWindow;
	WindowUniquePtr m_pCreateBridgeWindow;
	WindowUniquePtr m_pLogWindow;
	WindowUniquePtr m_pOverviewWindow;
	WindowUniquePtr m_pSettingsWindow;
	WindowUniquePtr m_pSummaryWindow;

	std::string m_LatestObfs4ProxyVersion;
	std::string m_LatestTorVersion;
	
	using BridgeMap = std::unordered_map<std::string, BridgeSharedPtr>;
	BridgeMap m_Bridges;
	using BridgeWindowMap = std::unordered_map<std::string, WindowUniquePtr>;
	BridgeWindowMap m_BridgeWindows;
	DeploymentUniquePtr m_pDeployment;
	MonitorUniquePtr m_pMonitor;
	GeolocationServiceUniquePtr m_pGeolocationService;
	VeilidNetworkUniquePtr m_pVeilidNetwork;
};

extern Turbine* g_pTurbine;

inline bool Turbine::IsActive() const
{
	return m_Active;
}

inline WebClient* Turbine::GetWebClient()
{
	return m_pWebClient.get();
}

inline Settings* Turbine::GetSettings() const
{
	return m_pConfiguration.get();
}

inline const std::string& Turbine::GetLatestTorVersion() const
{
	return m_LatestTorVersion;
}

inline const std::string& Turbine::GetLatestObfs4ProxyVersion() const
{
	return m_LatestObfs4ProxyVersion;
}

inline ProviderVector& Turbine::GetProviders()
{
	return m_Providers;
}

inline Window* Turbine::GetBridgesWindow()
{
	return m_pBridgesWindow.get();
}

inline Window* Turbine::GetCreateBridgeWindow()
{
	return m_pCreateBridgeWindow.get();
}

inline Window* Turbine::GetLogWindow()
{
	return m_pLogWindow.get();
}

inline Window* Turbine::GetOverviewWindow()
{
	return m_pOverviewWindow.get();
}

inline Window* Turbine::GetSettingsWindow()
{
	return m_pSettingsWindow.get();
}

inline Deployment* Turbine::GetDeployment() const
{
	return m_pDeployment.get();
}

inline Monitor* Turbine::GetMonitor() const
{
	return m_pMonitor.get();
}

inline GeolocationService* Turbine::GetGeolocationService() const
{
	return m_pGeolocationService.get();
}

} // namespace Turbine
