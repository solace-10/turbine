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

#include "bridge/statemachine.h"
#include "log.h"

namespace Turbine
{

StateMachine::StateMachine(const std::string& name) :
m_Name(name),
m_pCurrentState(nullptr)
{

}

const std::string& StateMachine::GetState() const
{
    static const std::string& nullState("no state");
    if (m_pCurrentState)
    {
        return m_pCurrentState->m_Name;
    }
    else
    {
        return nullState;
    }
}

void StateMachine::SetState(const std::string& stateName, bool force /* = false */)
{
    State* pState = FindState(stateName);
    if (pState == nullptr)
    {
        Log::Error("Unable to set state '%s' for state machine '%s': state doesn't exist.", stateName.c_str(), m_Name.c_str());
        return;
    }

    if (force)
    {
        m_pCurrentState = pState;
    }
    else
    {
        // WIP
        m_pCurrentState = pState;
    }
}

void StateMachine::AddState(const std::string& stateName)
{
    State* pState = FindState(stateName);
    if (pState != nullptr)
    {
        Log::Error("Unable to add state '%s' to state machine '%s': state already exists.", stateName.c_str(), m_Name.c_str());
        return;
    }

    StateUniquePtr pAddedState = std::make_unique<State>();
    pAddedState->m_Name = stateName;
    m_States.push_back(std::move(pAddedState));
}

void StateMachine::LinkStates(const std::string& fromStateName, const std::string& toStateName)
{

}

StateMachine::State* StateMachine::FindState(const std::string& stateName)
{
    for (auto& pState : m_States)
    {
        if (pState->m_Name == stateName)
        {
            return pState.get();
        }
    }

    return nullptr;
}

} // namespace Turbine
