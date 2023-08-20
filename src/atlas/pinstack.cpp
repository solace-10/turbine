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

#include "atlas/pinstack.hpp"

namespace Turbine
{

PinStack::PinStack(const ImVec2& position, const PinData& pinData) :
m_Position(position)
{
    m_Data.push_back(pinData);
}

void PinStack::Render()
{
    ImDrawList* pDrawList = ImGui::GetWindowDrawList();
    const float pinRadius = 8.0f;
    pDrawList->AddCircleFilled(m_Position, pinRadius, ImColor(0.0f, 1.0f, 1.0f, 1.0f));
    pDrawList->AddCircle(m_Position, pinRadius, ImColor(0.0f, 0.0f, 0.0f, 1.0f));
    
    size_t size = m_Data.size();
    if (size > 1)
    {
        std::stringstream ss;
        ss << size;
        m_Text = ss.str();

        ImVec2 textSize = ImGui::CalcTextSize(m_Text.c_str());
        ImVec2 textPosition;
        textPosition.x = m_Position.x - textSize.x / 2.0f;
        textPosition.y = m_Position.y - textSize.y / 2.0f;

        pDrawList->AddText(textPosition, ImColor(0.0f, 0.0f, 0.0f, 1.0f), m_Text.c_str());
    }
}

void PinStack::Add(const PinData& pinData)
{
    m_Data.push_back(pinData);
}

} // namespace Turbine
