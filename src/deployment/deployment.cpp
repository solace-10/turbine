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
#include "deployment/deployment.h"
#include "providers/provider.h"
#include "windows/logwindow.hpp"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

Deployment::Deployment() :
AnsibleCommand("deployment"),
m_ParsingResults(false)
{

}
    
Deployment::~Deployment()
{

}

void Deployment::Update(float delta)
{
    BridgeWeakPtrList pendingDeployments = GetPendingDeployments();
    if (m_pAnsibleCommand == nullptr && pendingDeployments.size() > 0)
    {
        ExecuteDeployments(pendingDeployments);
    }
    else if (m_pAnsibleCommand)
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

void Deployment::ExecuteDeployments(const BridgeWeakPtrList& pendingDeployments)
{
    GenerateInventory();

    for (auto& pPendingDeployment : pendingDeployments)
    {
        BridgeSharedPtr pBridge = pPendingDeployment.lock();
        if (pBridge)
        {
            pBridge->SetDeploymentState(Bridge::DeploymentState::Deploying);
        }
    }

    m_ParsingResults = false;
    m_pAnsibleCommand = std::make_unique<ShellCommand>(
        GetAnsibleCommand(),
        std::bind(&Deployment::OnDeploymentCommandFinished, this, std::placeholders::_1),
        std::bind(&Deployment::OnDeploymentCommandStandardOutput, this, std::placeholders::_1),
        std::bind(&Deployment::OnDeploymentCommandErrorOutput, this, std::placeholders::_1)
    );
    m_pAnsibleCommand->Run();
}

std::string Deployment::GetAnsibleCommand() const
{
    std::stringstream cmd;
    cmd << "ANSIBLE_HOST_KEY_CHECKING=False ansible-playbook -i ~/.local/share/turbine/inventory $(pwd)/data/ansible/deploybridge.yaml -e '{\"servers\": [\"DeploymentPending\"]}'";
    return cmd.str();
}

void Deployment::OnBridgeAdded(BridgeSharedPtr& pBridge)
{
    m_Deployments.push_back(pBridge);
}

Deployment::BridgeWeakPtrList Deployment::GetPendingDeployments() const
{
    BridgeWeakPtrList pendingDeployments;

    for (auto& pDeployment : m_Deployments)
    {
        BridgeSharedPtr pBridge = pDeployment.lock();
        if (pBridge && pBridge->GetDeploymentState() == Bridge::DeploymentState::DeploymentPending)
        {
            pendingDeployments.push_back(pDeployment);
        }
    }

    return pendingDeployments;
}

void Deployment::OnDeploymentComplete(Bridge* pBridge, bool success)
{
    if (pBridge->GetDeploymentState() == Bridge::DeploymentState::Deploying)
    {
        if (success)
        {
            pBridge->SetDeploymentState(Bridge::DeploymentState::Deployed);
            pBridge->SetTorState(Bridge::TorState::Running);
            pBridge->GetProvider()->OnBridgeDeployed(pBridge);
        }
        else
        {
            pBridge->SetDeploymentState(Bridge::DeploymentState::DeploymentFailed);
            pBridge->SetTorState(Bridge::TorState::Unknown);
        }
    }
}

void Deployment::OnDeploymentCommandFinished(int result)
{

}

void Deployment::OnDeploymentCommandStandardOutput(const std::string& output)
{
    AnsibleCommand::OnDeploymentCommandStandardOutput(output);

    LogWindow* pWindow = reinterpret_cast<LogWindow*>(g_pTurbine->GetLogWindow());
    pWindow->AddOutput("Deployment", output + "\n");

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

void Deployment::OnDeploymentCommandErrorOutput(const std::string& output)
{
    AnsibleCommand::OnDeploymentCommandErrorOutput(output);

    LogWindow* pWindow = reinterpret_cast<LogWindow*>(g_pTurbine->GetLogWindow());
    pWindow->AddOutput("Deployment", output + "\n");
}

bool Deployment::GetSuccessFromOutput(const std::string& output) const
{
    return output.find("failed=0") != std::string::npos;
}

} // namespace Turbine
