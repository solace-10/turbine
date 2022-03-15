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
#include <sstream>

#include <curl/curl.h>
#include <SDL.h>

#include "bridge/bridge.h"
#include "deployment/deployment.h"
#include "ext/json.hpp"
#include "imgui/imgui.h"
#include "providers/digitalocean/digitaloceanprovider.h"
#include "tasks/googlesearch/googlesearch.h"
#include "tasks/task.h"
#include "webclient/webclient.h"
#include "windows/bridgeswindow.h"
#include "windows/createbridgewindow.h"
#include "windows/deploymentwindow.hpp"
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
m_Searching(false),
m_Active(true)
{
	g_pTurbine = this;

	m_pConfiguration = std::make_unique<Settings>();
	InitialiseLoggers(pWindow);

	TextureLoader::Initialise();

	m_pBridgesWindow = std::make_unique<BridgesWindow>();
	m_pDeployment = std::make_unique<Deployment>();
	m_pCreateBridgeWindow = std::make_unique<CreateBridgeWindow>();
	m_pDeploymentWindow = std::make_unique<DeploymentWindow>();
	m_pSettingsWindow = std::make_unique<SettingsWindow>();
	m_pSummaryWindow = std::make_unique<SummaryWindow>();
	m_pWebClient = std::make_unique<WebClient>();
	m_pRep = std::make_unique<TurbineRep>(pWindow);

	// All the geolocation data needs to be loaded before the cameras are, as every 
	// camera will try to associate its IP address with a geolocation entry.
	InitialiseGeolocation();
	InitialiseCameras();
	InitialiseProviders();
    InitialiseTasks();
}

Turbine::~Turbine()
{
	m_Tasks.clear();
	m_Active = false;
}

void Turbine::InitialiseLoggers(SDL_Window* pWindow)
{
	Log::AddLogTarget(std::make_shared<FileLogger>("log.txt"));
#ifdef _WIN32
	Log::AddLogTarget(std::make_shared<VisualStudioLogger>());
#endif

	m_pNotificationLogger = std::make_shared<NotificationLogger>(pWindow);
	Log::AddLogTarget(m_pNotificationLogger);
}

void Turbine::InitialiseProviders()
{
	m_Providers.emplace_back(std::make_unique<DigitalOceanProvider>());
}

void Turbine::InitialiseGeolocation()
{

}

void Turbine::InitialiseCameras()
{

}

void Turbine::InitialiseTasks()
{
    m_Tasks.emplace_back(std::make_unique<Task>("Geolocation"));
    m_Tasks.emplace_back(std::make_unique<Task>("HTTP camera detector"));
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
	m_pWebClient->Update();
	m_pBridgesWindow->Update(delta);
	m_pCreateBridgeWindow->Update(delta);
	m_pDeploymentWindow->Update(delta);
	m_pSettingsWindow->Update(delta);
	m_pSummaryWindow->Update(delta);

	for (auto& pProvider : m_Providers)
	{
		pProvider->Update(delta);
	}

	for (auto& pTask : m_Tasks)
	{
		pTask->Update(delta);
	}

	m_pRep->Update(delta);
	m_pNotificationLogger->Update(delta);
	m_pRep->Render();
	m_pBridgesWindow->Render();
	m_pCreateBridgeWindow->Render();
	m_pDeploymentWindow->Render();
	m_pSettingsWindow->Render();
	m_pSummaryWindow->Render();
	m_pNotificationLogger->Render();
}

Task* Turbine::GetTask(const std::string& name) const
{
	for (auto&& pTask : m_Tasks)
	{
		if (pTask->GetName() == name)
		{
			return pTask.get();
		}
	}

	return nullptr;
}

void Turbine::OnMessageReceived(const json& message)
{
	const std::string& messageType = message["type"];
	if (messageType == "log")
	{
		const std::string& messageLevel = message["level"];
		const std::string& messagePlugin = message["plugin"];
		const std::string& messageText = message["message"];
		if (messageLevel == "warning") Log::Warning("%s %s", messagePlugin.c_str(), messageText.c_str());
		else if (messageLevel == "error") Log::Error("%s %s", messagePlugin.c_str(), messageText.c_str());
		else Log::Info("%s %s", messagePlugin.c_str(), messageText.c_str());
	}
	else if (messageType == "geolocation_result")
	{
		AddGeolocationData(message);
	}
	else if (messageType == "http_server_scan_result")
	{
	}
	else if (messageType == "stream_started")
	{
		CameraSharedPtr pCamera = FindCamera(message["url"]);
		if (pCamera != nullptr)
		{
			ChangeCameraState(pCamera, Camera::State::StreamAvailable);
		}
	}
}

CameraSharedPtr Turbine::FindCamera(const std::string& url)
{
	std::scoped_lock lock(m_CamerasMutex);
	for (CameraSharedPtr& pCamera : m_Cameras)
	{
		if (pCamera->GetURL() == url)
		{
			return pCamera;
		}
	}
	return CameraSharedPtr();
}

void Turbine::ChangeCameraState(CameraSharedPtr pCamera, Camera::State state)
{
	pCamera->SetState(state);
}

void Turbine::AddGeolocationData(const json& message)
{
	std::string addressStr = message["address"];
	const IPAddress address(addressStr);
	GeolocationDataSharedPtr pGeolocationData = std::make_shared<GeolocationData>(address);
	pGeolocationData->LoadFromJSON(message);
	Log::Info("Added geolocation data for %s: %s, %s", addressStr.c_str(), pGeolocationData->GetCity().c_str(), pGeolocationData->GetCountry().c_str());

	{
		std::scoped_lock lock(m_GeolocationDataMutex, m_CamerasMutex);
		m_GeolocationData[addressStr] = pGeolocationData;

		for (CameraSharedPtr& camera : m_Cameras)
		{
			if (camera->GetAddress().ToString() == pGeolocationData->GetIPAddress().ToString())
			{
				camera->SetGeolocationData(pGeolocationData);
			}
		}
	}
}

void Turbine::SetSearching(bool state)
{
	m_Searching = state;

	for (auto& pTask : m_Tasks)
	{
		if (m_Searching)
		{
			pTask->Start();
		}
		else
		{
			pTask->Stop();
		}
	}
}

void Turbine::AddBridge(BridgeSharedPtr&& pBridge)
{
	SDL_assert(m_Bridges.find(pBridge->GetId()) == m_Bridges.end());
	m_Bridges[pBridge->GetId()] = pBridge;
	m_pDeployment->OnBridgeAdded(pBridge);
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

} // namespace Turbine