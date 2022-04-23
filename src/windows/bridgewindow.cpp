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

#include <sstream>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <implot/implot.h>

#include "bridge/bridge.h"
#include "bridge/bridgegeolocation.hpp"
#include "bridge/bridgestats.hpp"
#include "providers/provider.h"
#include "windows/graphs/connectionsgraph.hpp"
#include "windows/graphs/percountrygraph.hpp"
#include "windows/bridgewindow.hpp"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

BridgeWindow::BridgeWindow(BridgeSharedPtr& pBridge)
{
    m_pBridge = pBridge;
    m_pConnectionsGraph = std::make_unique<ConnectionsGraph>(pBridge->GetStats());
    m_pPerCountryStatsGraph = std::make_unique<PerCountryGraph>(pBridge->GetStats());
}

BridgeWindow::~BridgeWindow()
{
    
}

void BridgeWindow::Render()
{
	if (IsOpen() == false)
	{
		return;
	}

    BridgeSharedPtr pBridge = m_pBridge.lock();
    if (pBridge == nullptr)
    {
        return;
    }

    std::stringstream windowName;
    windowName << pBridge->GetName() << " - details";

	ImGui::SetNextWindowSize(ImVec2(650, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(windowName.str().c_str(), &m_IsOpen))
	{
		ImGui::End();
		return;
	}

    std::string name = pBridge->GetName();
    ImGui::InputText("Name", &name, ImGuiInputTextFlags_ReadOnly);
    std::string ipv4 = pBridge->GetIPv4();
    ImGui::InputText("IPv4", &ipv4, ImGuiInputTextFlags_ReadOnly);
    std::string ipv6 = pBridge->GetIPv6();
    ImGui::InputText("IPv6", &ipv6, ImGuiInputTextFlags_ReadOnly);
    std::string distributionMethod = pBridge->GetDistributionMechanism();
    ImGui::InputText("Distribution method", &distributionMethod, ImGuiInputTextFlags_ReadOnly);
    std::string fingerprint = pBridge->GetFingerprint();
    ImGui::InputText("Fingerprint", &fingerprint, ImGuiInputTextFlags_ReadOnly);
    std::string hashedFingerprint = pBridge->GetHashedFingerprint();
    ImGui::InputText("Hashed fingerprint", &hashedFingerprint, ImGuiInputTextFlags_ReadOnly);

    RenderGeolocation();

    ImGui::TextUnformatted("Stats:");

    m_pConnectionsGraph->Render();
    m_pPerCountryStatsGraph->Render();

	ImGui::End();
}

void BridgeWindow::RenderGeolocation()
{
    BridgeGeolocation* pGeolocation = m_pBridge.lock()->GetGeolocation();
    if (pGeolocation != nullptr)
    {
        ImGui::TextUnformatted("Location:");
        std::string country = pGeolocation->GetCountry();
        ImGui::InputText("Country", &country, ImGuiInputTextFlags_ReadOnly);
        std::string city = pGeolocation->GetCity();
        ImGui::InputText("City", &city, ImGuiInputTextFlags_ReadOnly);
        std::string organisation = pGeolocation->GetOrganisation();
        ImGui::InputText("Organisation", &organisation, ImGuiInputTextFlags_ReadOnly);
    }
}

} // namespace Turbine
