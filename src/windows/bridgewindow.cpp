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
#include "bridge/bridgestats.hpp"
#include "providers/provider.h"
#include "windows/bridgewindow.hpp"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

BridgeWindow::BridgeWindow(BridgeSharedPtr& pBridge)
{
    m_pBridge = pBridge;
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

    ImGui::Text("Name %s", pBridge->GetName().c_str());

    ImGui::Text("IPv4: %s", pBridge->GetIPv4().c_str());
    ImGui::Text("IPv6: %s", pBridge->GetIPv6().c_str());

    ImGui::Text("Distribution mechanism: %s", pBridge->GetDistributionMechanism().c_str());

    std::string fingerprint = pBridge->GetFingerprint();
    ImGui::InputText("Fingerprint", &fingerprint, ImGuiInputTextFlags_ReadOnly);
    std::string hashedFingerprint = pBridge->GetHashedFingerprint();
    ImGui::InputText("Hashed fingerprint", &hashedFingerprint, ImGuiInputTextFlags_ReadOnly);

    ImGui::TextUnformatted("Stats:");

    BridgeStatsSharedPtr pStats = pBridge->GetStats();
    pStats->RenderPerCountryStats();

	ImGui::End();
}

} // namespace Turbine
