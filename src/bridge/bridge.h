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

#pragma once

#include <memory>
#include <string>

#include "bridgesummarywidget.h"

namespace Turbine
{

class BridgeSummaryWidget;
using BridgeSummaryWidgetUniquePtr = std::unique_ptr<BridgeSummaryWidget>;

class Bridge;
using BridgeWeakPtr = std::weak_ptr<Bridge>;
using BridgeSharedPtr = std::shared_ptr<Bridge>;

class StateMachine;
using StateMachineUniquePtr = std::unique_ptr<StateMachine>;

class Bridge
{
public:
    Bridge(const std::string& id, const std::string& name, const std::string& state);
    ~Bridge();

    const std::string& GetId() const;
    const std::string& GetName() const;
    const std::string& GetState() const;
    const std::string& GetIPv4() const;
    const std::string& GetIPv6() const;
    void SetIPv4(const std::string& ip);
    void SetIPv6(const std::string& ip);
    unsigned int GetORPort() const;
    unsigned int GetExtPort() const;
    void SetORPort(unsigned int port);
    void SetExtPort(unsigned int port);
    void SetState(const std::string& state, bool force = false);

    void RenderSummaryWidget();

private:
    void InitialiseStateMachine();

    std::string m_Id;
    std::string m_Name;
    BridgeSummaryWidgetUniquePtr m_pBridgeSummaryWidget;
    StateMachineUniquePtr m_pStateMachine;
    std::string m_Ipv4;
    std::string m_Ipv6;
    unsigned int m_ORPort;
    unsigned int m_ExtPort;
};

} // namespace Turbine