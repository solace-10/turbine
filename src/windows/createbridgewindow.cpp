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

#include <cctype>
#include <stdlib.h>
#include <time.h>

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "json.hpp"

#include "providers/provider.h"
#include "windows/createbridgewindow.h"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

CreateBridgeWindow::CreateBridgeWindow() :
m_BridgeName("TurbineUnnamed")
{
	srand(time(nullptr));
	LoadBridgeNames();
}

void CreateBridgeWindow::Render()
{
	if (IsOpen() == false)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(650, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Create bridge", &m_IsOpen))
	{
		ImGui::End();
		return;
	}

	ImGui::InputText("Bridge name", &m_BridgeName);

	bool canCreateBridge = true;
	if (IsValidBridgeName(m_BridgeName) == false)
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid bridge name. Must be 1-19 characters and alphanumeric.");
		canCreateBridge = false;
	}

	if (ImGui::Button("Create") && canCreateBridge)
	{
		ProviderVector& providers = g_pTurbine->GetProviders();
		providers[0]->CreateBridge(m_BridgeName, true);
	}

	ImGui::End();
}

void CreateBridgeWindow::OnOpen()
{
	if (m_BridgeNames.empty() == false)
	{
		m_BridgeName = "Turbine" + m_BridgeNames[rand() % m_BridgeNames.size()];
	}
}

void CreateBridgeWindow::LoadBridgeNames()
{
	using json = nlohmann::json;
	std::filesystem::path filePath = "data/names.json";
	std::ifstream file(filePath);
	if (file.is_open())
	{
		json names;
		file >> names;
		file.close();

		if (names.is_array())
		{
			const size_t numNames = names.size();
			m_BridgeName.reserve(numNames);
			for (size_t i = 0; i < numNames; ++i)
			{
				m_BridgeNames.push_back(names[i].get<std::string>());
			}
		}
	}
}

bool CreateBridgeWindow::IsValidBridgeName(const std::string& name) const
{
	// Valid names must be between 1 and 19 characters and must only
	// contain [a-zA-Z0-9].
    if (name.size() < 1 || name.size() > 19)
	{
		return false;
	}

	for (char c : name)
	{
		if (isalnum(c) == 0)
		{
			return false;
		}
	}

	return true;
}

} // namespace Turbine
