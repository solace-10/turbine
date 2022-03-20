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

#include "bridge/bridge.h"
#include "bridge/bridgesummarywidget.h"
#include "imgui/imgui.h"
#include "imguiext/widgets.h"
#include "windows/window.h"
#include "icons.h"
#include "turbine.h"

namespace Turbine
{

BridgeSummaryWidget::BridgeSummaryWidget()
{
    m_SpinnerTimer = static_cast<float>(rand() % 1000) / 1000.f;
}

void BridgeSummaryWidget::Update(float delta)
{
    m_SpinnerTimer += delta;
}

void BridgeSummaryWidget::Render(Bridge* pBridge)
{
    Update(ImGui::GetIO().DeltaTime);

    using namespace ImGui;
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    BeginChild(pBridge->GetId().c_str(), ImVec2(0.0f, 40.0f), true);

    ImVec2 pos = GetCursorScreenPos();
    
    if (pBridge->GetState() == "Deployed")
	{
        float timer = m_SpinnerTimer / 2.0f;
		SetCursorScreenPos(ImVec2(pos.x + 8, pos.y + 4));
		SpinnerFilled(10.0f, 4, IM_COL32(0, 255, 255, 255), &timer);
		SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 4));
		Text("%s", pBridge->GetName().c_str());
		SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 20));
		TextUnformatted(pBridge->GetState().c_str());
	}
    else
    {
        SetCursorScreenPos(ImVec2(pos.x + 8, pos.y + 4));
        Spinner(10.0f, 4, IM_COL32(0, 255, 255, 255), &m_SpinnerTimer);
        SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 4));
        Text("%s", pBridge->GetName().c_str());
        SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 20));
        TextUnformatted(pBridge->GetState().c_str());
    }

    SetCursorScreenPos(pos);
    if (ImGui::InvisibleButton("BridgeSummaryButton", ImGui::GetWindowSize()))
    {
        Window* pWindow = g_pTurbine->GetBridgeWindow(pBridge);
        if (pWindow != nullptr)
        {
            pWindow->Show(true);
        }
    }

    EndChild();
    PopStyleVar();
}

} // namespace Turbine