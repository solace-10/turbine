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

#include "imgui/imgui.h"
#include "imguiext/widgets.h"
#include "tasks/task.h"
#include "bridge.h"
#include "bridgesummarywidget.h"
#include "icons.h"

namespace Turbine
{

BridgeSummaryWidget::BridgeSummaryWidget() :
m_SpinnerTimer(0.0f)
{

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
    
    // if (GetState() == State::Idle)
    // {
    //     SetCursorScreenPos(ImVec2(pos.x + 8, pos.y + 4));
    //     Spinner(10.0f, 4, IM_COL32(120, 120, 120, 255), &m_SpinnerTimer);
    //     SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 4));
    //     TextDisabled("%s", m_Name.c_str());
    //     SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 20));
    //     TextDisabled("Idle");
    // }
	// else if (GetState() == State::Disabled)
	// {
	// 	SetCursorScreenPos(ImVec2(pos.x + 6, pos.y + 6));
    //     Image(reinterpret_cast<ImTextureID>(uintptr_t(Icons::GetIcon(IconId::TaskDisabled))), ImVec2(26, 26));
	// 	SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 4));
	// 	TextDisabled("%s", m_Name.c_str());
	// 	SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 20));
	// 	TextDisabled("Disabled");
	// }
	// else if (GetState() == State::Error)
	// {
	// 	SetCursorScreenPos(ImVec2(pos.x + 6, pos.y + 6));
    //     Image(reinterpret_cast<ImTextureID>(uintptr_t(Icons::GetIcon(IconId::TaskError))), ImVec2(26, 26), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1));
	// 	SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 4));
	// 	TextColored(ImVec4(1, 0, 0, 1), "%s", m_Name.c_str());
	// 	SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 20));
    //     if (m_Error.empty())
    //     {
    //         TextDisabled("Error");
    //     }
    //     else
    //     {
    //         TextColored(ImVec4(1, 0, 0, 1), "%s", m_Error.c_str());
    //     }
	// }
    // else
    {
        SetCursorScreenPos(ImVec2(pos.x + 8, pos.y + 4));
        Spinner(10.0f, 4, IM_COL32(0, 255, 255, 255), &m_SpinnerTimer);
        SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 4));
        Text("%s", pBridge->GetName().c_str());
        SetCursorScreenPos(ImVec2(pos.x + 38, pos.y + 20));
        Text("Running on localhost");
    }

    EndChild();
    PopStyleVar();
}

} // namespace Turbine