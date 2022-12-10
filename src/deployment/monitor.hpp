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

#include <chrono>
#include <list>
#include <string>

#include "bridge/bridge.h"
#include "deployment/ansiblecommand.h"

namespace Turbine
{

class ShellCommand;
using ShellCommandUniquePtr = std::unique_ptr<ShellCommand>;

class Monitor : public AnsibleCommand
{
public:
    Monitor();
    ~Monitor();

    virtual void OnDeploymentCommandOutput(const std::string& output) override;
    virtual void OnSuccess(Bridge* pBridge) override;
    virtual void OnUnreachable(Bridge* pBridge, const std::string& error) override;
    virtual void OnFailed(Bridge* pBridge, const std::string& error) override;

    void Update(float delta);
    void Retrieve();

private:
    using BridgeWeakPtrList = std::list<BridgeWeakPtr>;

    void ExecuteDeployments(const BridgeWeakPtrList& pendingDeployments);
    std::string GetAnsibleCommand() const;
    void OnDeploymentCommandFinished(int result);

    ShellCommandUniquePtr m_pAnsibleCommand;
    bool m_ParsingResults;

    std::chrono::time_point<std::chrono::system_clock> m_NextRetrieval;
    std::chrono::hours m_RetrivalInterval;
};

} // namespace Turbine
