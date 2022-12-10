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
    using namespace std::literals;
    m_RetrivalInterval = 4h;
    m_NextRetrieval = std::chrono::system_clock::now() + m_RetrivalInterval;
}
    
Monitor::~Monitor()
{

}

void Monitor::Update(float delta)
{
    if (m_NextRetrieval < std::chrono::system_clock::now())
    {
        GenerateInventory();
        m_ParsingResults = false;
        m_pAnsibleCommand = std::make_unique<ShellCommand>(
            GetAnsibleCommand(),
            std::bind(&Monitor::OnDeploymentCommandFinished, this, std::placeholders::_1),
            std::bind(&Monitor::OnDeploymentCommandOutput, this, std::placeholders::_1)
        );
        m_pAnsibleCommand->Run();
        m_NextRetrieval = std::chrono::system_clock::now() + m_RetrivalInterval;
    }

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

void Monitor::Retrieve()
{
    m_NextRetrieval = std::chrono::system_clock::now();
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
    cmd << "ANSIBLE_HOST_KEY_CHECKING=False ansible-playbook -i ~/.local/share/turbine/inventory $(pwd)/data/ansible/monitor.yaml -e '{\"servers\": [\"Deployed\"]}'";
    return cmd.str();
}

void Monitor::OnDeploymentCommandFinished(int result)
{

}

void Monitor::OnDeploymentCommandOutput(const std::string& output)
{
    AnsibleCommand::OnDeploymentCommandOutput(output);
    
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
            pBridge->OnMonitoredDataUpdated();
        }
    }
}

void Monitor::OnSuccess(Bridge* pBridge) 
{
    pBridge->OnMonitoredDataUpdated();
}

void Monitor::OnUnreachable(Bridge* pBridge, const std::string& error)
{
    pBridge->SetTorState(Bridge::TorState::Unreachable);
}

void Monitor::OnFailed(Bridge* pBridge, const std::string& error) 
{
    pBridge->SetTorState(Bridge::TorState::Unknown);
}

} // namespace Turbine
