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

#include <array>

#include "bridge/bridge.h"
#include "bridge/bridgesummarywidget.h"
#include "bridge/statemachine.h"
#include "deployment/deployment.h"
#include "turbine.h"

namespace Turbine
{

Bridge::Bridge(const std::string& id, const std::string& name, const std::string& initialState) :
m_Id(id),
m_Name(name),
m_ORPort(0),
m_ExtPort(0)
{
    m_pBridgeSummaryWidget = std::make_unique<BridgeSummaryWidget>();
    m_pStateMachine = std::make_unique<StateMachine>(name);
    InitialiseStateMachine();
    m_pStateMachine->SetState(initialState, true);
}

Bridge::~Bridge()
{
}

const std::string& Bridge::GetId() const
{
    return m_Id;
}

const std::string& Bridge::GetName() const
{
    return m_Name;
}

const std::string& Bridge::GetState() const
{
    return m_pStateMachine->GetState();
}
    
void Bridge::SetState(const std::string& state, bool force /* = false */)
{
    m_pStateMachine->SetState(state, force);
}

const std::string& Bridge::GetIPv4() const
{
    return m_Ipv4;
}

const std::string& Bridge::GetIPv6() const
{
    return m_Ipv6;
}

void Bridge::SetIPv4(const std::string& ip)
{
    m_Ipv4 = ip;
}

void Bridge::SetIPv6(const std::string& ip)
{
    m_Ipv6 = ip;
}

unsigned int Bridge::GetORPort() const
{
    return m_ORPort;
}

unsigned int Bridge::GetExtPort() const
{
    return m_ExtPort;
}

void Bridge::SetORPort(unsigned int port)
{
    m_ORPort = port;
}

void Bridge::SetExtPort(unsigned int port)
{
    m_ExtPort = port;
}

void Bridge::RenderSummaryWidget()
{
    m_pBridgeSummaryWidget->Render(this);
}

void Bridge::InitialiseStateMachine()
{
    m_pStateMachine->AddState("Offline");
    m_pStateMachine->AddState("Shutting down");
    m_pStateMachine->AddState("New");
    m_pStateMachine->AddState("Deployment pending");
    m_pStateMachine->AddState("Deploying");
    m_pStateMachine->AddState("Deployed");

    m_pStateMachine->LinkStates("Offline", "New");

    m_pStateMachine->LinkStates("Shutting down", "Offline");

	m_pStateMachine->LinkStates("New", "Shutting down");
	m_pStateMachine->LinkStates("New", "Offline");
    m_pStateMachine->LinkStates("New", "Deployment needed");
    m_pStateMachine->LinkStates("New", "Deploying");
}

} // namespace Turbine