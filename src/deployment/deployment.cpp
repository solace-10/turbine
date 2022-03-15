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
#include "windows/deploymentwindow.hpp"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

Deployment::Deployment() :
m_BridgesChanged(false)
{
}
    
Deployment::~Deployment()
{

}

void Deployment::Update(float delta)
{
    if (m_BridgesChanged)
    {
        GenerateHostsFile();
        m_BridgesChanged = false;
    }

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
    for (auto& pPendingDeployment : pendingDeployments)
    {
        BridgeSharedPtr pBridge = pPendingDeployment.lock();
        if (pBridge)
        {
            pBridge->SetState("Deploying");
        }
    }

    m_pAnsibleCommand = std::make_unique<ShellCommand>(
        GetAnsibleCommand(),
        std::bind(&Deployment::OnDeploymentComplete, this, std::placeholders::_1),
        std::bind(&Deployment::OnDeploymentOutput, this, std::placeholders::_1)
    );
    m_pAnsibleCommand->Run();
}

std::string Deployment::GetAnsibleCommand() const
{
    std::stringstream cmd;
    cmd << "/home/hostilenode/.local/bin/ansible-playbook -i /home/hostilenode/.local/share/turbine/ansiblehosts /home/hostilenode/Dev/turbine/bin/data/ansible/deploybrige.yaml -e '{\"servers\": [\"all\"]}'";
    return cmd.str();
}

void Deployment::OnBridgeAdded(BridgeSharedPtr& pBridge)
{
    m_Deployments.push_back(pBridge);
    m_BridgesChanged = true;
}

void Deployment::OnBridgeIpChanged()
{
    m_BridgesChanged = true;
}

void Deployment::GenerateHostsFile()
{
    std::filesystem::path storagePath = g_pTurbine->GetSettings()->GetStoragePath();

	std::filesystem::path filePath = storagePath / "ansiblehosts";
	std::ofstream file(filePath, std::ofstream::out | std::ofstream::trunc);
    if (file.good())
    {
		for (auto& pDeployments : m_Deployments)
		{
			BridgeSharedPtr pBridge = pDeployments.lock();
			if (pBridge != nullptr)
			{
                if (pBridge->GetIPv4().empty() && pBridge->GetIPv6().empty())
                {
                    continue;
                }

				file << "[" << pBridge->GetName() << "]\n";
                if (pBridge->GetIPv4().empty() == false)
                {
                    file << pBridge->GetIPv4();
                }
                else
                {
                    file << pBridge->GetIPv6();
                }

                file << " ansible_user=root";
                file << " host_key_checking=False";
                file << " tor_port=" << pBridge->GetORPort();
                file << " obfs4_port=" << pBridge->GetExtPort();
                file << "\n\n";
			}
		}
        file.close();
    }
}

Deployment::BridgeWeakPtrList Deployment::GetPendingDeployments() const
{
    BridgeWeakPtrList pendingDeployments;

    for (auto& pDeployment : m_Deployments)
    {
        BridgeSharedPtr pBridge = pDeployment.lock();
        if (pBridge && pBridge->GetState() == "Deployment pending")
        {
            pendingDeployments.push_back(pDeployment);
        }
    }

    return pendingDeployments;
}

void Deployment::OnDeploymentComplete(int result)
{
    int a = 0;
}

void Deployment::OnDeploymentOutput(const std::string& output)
{
    DeploymentWindow* pWindow = reinterpret_cast<DeploymentWindow*>(g_pTurbine->GetDeploymentWindow());
    pWindow->AddOutput(output + "\n");
}

} // namespace Turbine
