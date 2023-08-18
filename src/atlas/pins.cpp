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

#include "atlas/atlas.h"
#include "atlas/pins.hpp"
#include "atlas/pinstack.hpp"
#include "bridge/bridge.h"
#include "bridge/bridgegeolocation.hpp"
#include "turbine.h"

namespace Turbine
{

Pins::Pins(Atlas* pAtlas) :
m_pAtlas(pAtlas)
{

}

Pins::~Pins()
{

}

void Pins::Update(float delta)
{
    m_PinStacks.clear();

    const float aggregationThreshold = 32.0f;
    const float aggregationThresholdSquared = aggregationThreshold * aggregationThreshold;
    for (Bridge* pBridge : g_pTurbine->GetBridges())
    {
        BridgeGeolocation* pGeolocation = pBridge->GetGeolocation();
        if (pGeolocation == nullptr)
        {
            continue;
        }

        bool pinStackFound = false;
        ImVec2 pinPosition = m_pAtlas->GetScreenCoordinates(pGeolocation->GetLongitude(), pGeolocation->GetLatitude());
        for (PinStackUniquePtr& pPinStack : m_PinStacks)
        {
            const ImVec2 pinStackPosition = pPinStack->GetPosition();
            if (DistanceBetweenSquared(pinPosition, pinStackPosition) < aggregationThresholdSquared)
            {
                pPinStack->AddBridge(pBridge);
                pinStackFound = true;
                break;
            }
        }

        if (pinStackFound == false)
        {
            m_PinStacks.push_back(std::move(std::make_unique<PinStack>(pinPosition, pBridge)));
        }
    }
}
    
void Pins::Render()
{
    for (auto& pPinStack : m_PinStacks)
    {
        pPinStack->Render();
    }
}

float Pins::DistanceBetweenSquared(const ImVec2& p1, const ImVec2& p2) const
{
    ImVec2 diff(p2.x - p1.x, p2.y - p1.y);
    return diff.x * diff.x + diff.y + diff.y;
}

}