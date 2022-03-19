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

#include <filesystem>
#include <fstream>

#include "bridge/bridge.h"
#include "core/shellcommand/shellcommand.hpp"
#include "deployment/monitor.hpp"
#include "providers/provider.h"
#include "windows/deploymentwindow.hpp"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

Monitor::Monitor() :
m_ParsingResults(false)
{

}
    
Monitor::~Monitor()
{

}

void Monitor::Update(float delta)
{
    if (m_pAnsibleCommand)
    {
        m_pAnsibleCommand->Update();
        if (m_pAnsibleCommand->GetState() == ShellCommand::State::Completed)
        {
            m_pAnsibleCommand = nullptr;
        }
        else if (m_pAnsibleCommand->GetState() == ShellCommand::State::FailedToRun)
        {
            Log::Error("Deployment command failed.");
            m_pAnsibleCommand = nullptr;
        }
    }
}

void Monitor::ExecuteDeployments(const BridgeWeakPtrList& pendingDeployments)
{
    GenerateInventory();

    m_ParsingResults = false;
    m_pAnsibleCommand = std::make_unique<ShellCommand>(
        GetAnsibleCommand(),
        std::bind(&Monitor::OnDeploymentCommandFinished, this, std::placeholders::_1),
        std::bind(&Monitor::OnDeploymentCommandOutput, this, std::placeholders::_1)
    );
    m_pAnsibleCommand->Run();
}

std::string Monitor::GetAnsibleCommand() const
{
    std::stringstream cmd;
    cmd << "/home/hostilenode/.local/bin/ansible-playbook -i /home/hostilenode/.local/share/turbine/inventory /home/hostilenode/Dev/turbine/bin/data/ansible/deploybrige.yaml -e '{\"servers\": [\"DeploymentPending\"]}'";
    return cmd.str();
}

void Monitor::OnDeploymentComplete(Bridge* pBridge, bool success)
{
    if (pBridge->GetState() == "Deploying")
    {
        if (success)
        {
            pBridge->SetState("Deployed");
            pBridge->GetProvider()->OnBridgeDeployed(pBridge);
        }
        else
        {
            pBridge->SetState("Deployment failed");
        }
    }
}

void Monitor::OnDeploymentCommandFinished(int result)
{

}

void Monitor::OnDeploymentCommandOutput(const std::string& output)
{
    DeploymentWindow* pWindow = reinterpret_cast<DeploymentWindow*>(g_pTurbine->GetDeploymentWindow());
    pWindow->AddOutput(output + "\n");

    if (m_ParsingResults == false && output.rfind("PLAY RECAP *", 0) == 0)
    {
        m_ParsingResults = true;
    }
    else if (m_ParsingResults)
    {
        Bridge* pBridge = GetBridgeFromOutput(output);
        if (pBridge != nullptr)
        {
            OnDeploymentComplete(pBridge, GetSuccessFromOutput(output));
        }
    }
}

Bridge* Monitor::GetBridgeFromOutput(const std::string& output) const
{
    size_t ipEndIdx = output.find(' ');
    if (ipEndIdx != std::string::npos)
    {
        const std::string ip = output.substr(0, ipEndIdx);
        for (BridgeWeakPtr pBridgeWeak : m_Deployments)
        {
            BridgeSharedPtr pBridge = pBridgeWeak.lock();
            if (pBridge && (ip == pBridge->GetIPv4() || ip == pBridge->GetIPv6()))
            {
                return pBridge.get();
            }
        }
    }
    return nullptr;
}

bool Monitor::GetSuccessFromOutput(const std::string& output) const
{
    return output.find("failed=0") != std::string::npos;
}

} // namespace Turbine
