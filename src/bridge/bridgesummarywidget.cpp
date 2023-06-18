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
    const ImVec2 pos = GetCursorScreenPos();
    RenderIcon(pBridge);

    SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 4));
    Text("%s", pBridge->GetName().c_str());
    SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 20));
    TextUnformatted(pBridge->GetStateText().c_str());

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

void BridgeSummaryWidget::RenderIcon(Bridge* pBridge)
{
    using namespace ImGui;

    ImVec2 pos = GetCursorScreenPos();

    bool warningsPresent = false;
    bool errorsPresent = false;
    for (auto& pIssue : pBridge->GetBridgeIssues())
    {
        if (pIssue->GetType() == BridgeIssue::Type::Warning)
        {
            warningsPresent = true;
        }
        else if (pIssue->GetType() == BridgeIssue::Type::Error)
        {
            errorsPresent = true;
            break;
        }
    }

    if (errorsPresent)
    {
        SetCursorScreenPos(ImVec2(pos.x + 7, pos.y + 6));
        Image(reinterpret_cast<ImTextureID>(Icons::GetIcon(IconId::Issue)), ImVec2(26, 26), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 0, 0, 1));    
    }
    else if (warningsPresent)
    {
        SetCursorScreenPos(ImVec2(pos.x + 7, pos.y + 6));
        Image(reinterpret_cast<ImTextureID>(Icons::GetIcon(IconId::Issue)), ImVec2(26, 26), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 0.5, 0, 1));          
    }
    else if (pBridge->GetTorState() == Bridge::TorState::Running)
    {
        SetCursorScreenPos(ImVec2(pos.x + 7, pos.y + 7));
        Image(reinterpret_cast<ImTextureID>(Icons::GetIcon(IconId::ServerSmall)), ImVec2(26, 26));   
    }
    else
    {
        float timer = m_SpinnerTimer / 2.0f;
        SetCursorScreenPos(ImVec2(pos.x + 8, pos.y + 4));
        SpinnerFilled(10.0f, 4, IM_COL32(0, 255, 255, 255), &timer);
    }
}

} // namespace Turbine