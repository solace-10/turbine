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

#include <chrono>
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>

#include <curl/curl.h>
#include <SDL.h>

#include "bridge/bridge.h"
#include "deployment/deployment.h"
#include "deployment/monitor.hpp"
#include "ext/json.hpp"
#include "imgui/imgui.h"
#include "providers/digitalocean/digitaloceanprovider.h"
#include "webclient/webclient.h"
#include "windows/bridgewindow.hpp"
#include "windows/bridgeswindow.h"
#include "windows/createbridgewindow.h"
#include "windows/logwindow.hpp"
#include "windows/overviewwindow.hpp"
#include "windows/settingswindow.h"
#include "windows/summarywindow.h"
#include "geolocationdata.h"
#include "log.h"
#include "turbine.h"
#include "turbinerep.h"
#include "textureloader.h"
#include "settings.h"

extern IMGUI_API ImGuiContext* GImGui;

namespace Turbine
{

Turbine* g_pTurbine = nullptr;

Turbine::Turbine(SDL_Window* pWindow, unsigned int scannerCount) :
m_Active(true)
{
	g_pTurbine = this;

	m_pConfiguration = std::make_unique<Settings>();
	InitialiseLoggers(pWindow);

	TextureLoader::Initialise();

	m_pBridgesWindow = std::make_unique<BridgesWindow>();
	m_pDeployment = std::make_unique<Deployment>();
	m_pMonitor = std::make_unique<Monitor>();
	m_pCreateBridgeWindow = std::make_unique<CreateBridgeWindow>();
	m_pLogWindow = std::make_unique<LogWindow>();
	m_pOverviewWindow = std::make_unique<OverviewWindow>();
	m_pSettingsWindow = std::make_unique<SettingsWindow>();
	m_pSummaryWindow = std::make_unique<SummaryWindow>();
	m_pWebClient = std::make_unique<WebClient>();
	m_pRep = std::make_unique<TurbineRep>(pWindow);

	InitialiseProviders();
	InitialiseLatestVersions();
}

Turbine::~Turbine()
{
	m_Active = false;
}

void Turbine::InitialiseLatestVersions()
{
	InitialiseLatestTorVersion();
	InitialiseLatestObfs4ProxyVersion();
}

void Turbine::InitialiseLatestTorVersion()
{

}

void Turbine::InitialiseLatestObfs4ProxyVersion()
{
	GetWebClient()->Get("https://gitlab.com/yawning/obfs4/-/raw/master/obfs4proxy/obfs4proxy.go", {},
		[this](const WebClientRequestResult& result)
		{
			const std::regex pattern("obfs4proxyVersion\\s*=\\s*\"(\\d+\\.\\d+\\.\\d+)\"");
			std::smatch match;
			if (std::regex_search(result.GetData(), match, pattern))
			{
				this->m_LatestObfs4ProxyVersion = match[1];
			}
		}
	);
}

void Turbine::InitialiseLoggers(SDL_Window* pWindow)
{
	Log::AddLogTarget(std::make_shared<FileLogger>("log.txt"));

	m_pNotificationLogger = std::make_shared<NotificationLogger>(pWindow);
	Log::AddLogTarget(m_pNotificationLogger);
}

void Turbine::InitialiseProviders()
{
	m_Providers.emplace_back(std::make_unique<DigitalOceanProvider>());
}

void Turbine::ProcessEvent(const SDL_Event& event)
{
	m_pRep->ProcessEvent(event);
}

void Turbine::Update()
{
	TextureLoader::Update();

	const float delta = ImGui::GetIO().DeltaTime;

	m_pDeployment->Update(delta);
	m_pMonitor->Update(delta);
	m_pWebClient->Update();
	m_pBridgesWindow->Update(delta);
	m_pCreateBridgeWindow->Update(delta);
	m_pLogWindow->Update(delta);
	m_pOverviewWindow->Update(delta);
	m_pSettingsWindow->Update(delta);
	m_pSummaryWindow->Update(delta);

	for (auto& pBridgeWindowPair : m_BridgeWindows)
	{
		pBridgeWindowPair.second->Update(delta);
	}

	for (auto& pProvider : m_Providers)
	{
		pProvider->Update(delta);
	}

	m_pRep->Update(delta);
	m_pNotificationLogger->Update(delta);
	m_pRep->Render();
	m_pBridgesWindow->Render();
	m_pCreateBridgeWindow->Render();
	m_pLogWindow->Render();
	m_pOverviewWindow->Render();
	m_pSettingsWindow->Render();
	m_pSummaryWindow->Render();
	m_pNotificationLogger->Render();

	for (auto& pBridgeWindowPair : m_BridgeWindows)
	{
		pBridgeWindowPair.second->Render();
	}
}

void Turbine::AddBridge(BridgeSharedPtr&& pBridge)
{
	SDL_assert(m_Bridges.find(pBridge->GetId()) == m_Bridges.end());
	m_Bridges[pBridge->GetId()] = pBridge;
	m_BridgeWindows[pBridge->GetId()] = std::make_unique<BridgeWindow>(pBridge);
	m_pDeployment->OnBridgeAdded(pBridge);
	reinterpret_cast<OverviewWindow*>(m_pOverviewWindow.get())->OnBridgeAdded();

	// Notify the monitor to retrieve monitored data from the new bridge.
	m_pMonitor->Retrieve();
}

Bridge* Turbine::GetBridge(const std::string& id)
{
	BridgeMap::iterator it = m_Bridges.find(id);
	return (it == m_Bridges.end()) ? nullptr : it->second.get();
}

const Bridge* Turbine::GetBridge(const std::string& id) const
{
	BridgeMap::const_iterator it = m_Bridges.find(id);
	return (it == m_Bridges.end()) ? nullptr : it->second.get();
}
	
BridgeList Turbine::GetBridges() const
{
	BridgeList bridges;
	for (auto& bridge : m_Bridges)
	{
		bridges.push_back(bridge.second.get());
	}
	return bridges;
}

Window* Turbine::GetBridgeWindow(Bridge* pBridge)
{
	if (pBridge == nullptr)
	{
		return nullptr;
	}
	else
	{
		BridgeWindowMap::iterator it = m_BridgeWindows.find(pBridge->GetId());
		if (it == m_BridgeWindows.end())
		{
			return nullptr;
		}
		else
		{
			return it->second.get();
		}
	}
}

} // namespace Turbine