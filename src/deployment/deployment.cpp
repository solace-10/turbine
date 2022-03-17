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
    for (auto& pPendingDeployment : pendingDeployments)
    {
        BridgeSharedPtr pBridge = pPendingDeployment.lock();
        if (pBridge)
        {
            pBridge->SetState("Deploying");
        }
    }

    GenerateHostsFile(pendingDeployments);

    m_ParsingResults = false;
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
}

void Deployment::GenerateHostsFile(const BridgeWeakPtrList& pendingDeployments)
{
    Settings* pSettings = g_pTurbine->GetSettings();
    std::filesystem::path storagePath = pSettings->GetStoragePath();

	std::filesystem::path filePath = storagePath / "ansiblehosts";
	std::ofstream file(filePath, std::ofstream::out | std::ofstream::trunc);
    if (file.good())
    {
		for (auto& pDeployments : pendingDeployments)
		{
			BridgeSharedPtr pBridge = pDeployments.lock();
			if (pBridge != nullptr)
			{
                if (pBridge->GetIPv4().empty() && pBridge->GetIPv6().empty())
                {
                    continue;
                }

                json additionalConfig = 
                {
                    {{"name", "PublishServerDescriptor"}, {"value", "bridge"}},
                    {{"name", "Log"}, {"value", "notice file /var/log/tor/notices.log"}}
                };

				file << "[" << pBridge->GetName() << "]\n";
                const std::string& ip = pBridge->GetIPv4().size() > 0 ? pBridge->GetIPv4() : pBridge->GetIPv6();
                file << ip;
                file << " nickname=" << pBridge->GetName();
                file << " ansible_user=root";
                file << " public_ip=" << ip;
                file << " email_address=" << pSettings->GetContactEmail();
                file << " tor_port=" << pBridge->GetORPort();
                file << " obfs4_port=" << pBridge->GetExtPort();
                file << " additionnal_torrc_config='" << additionalConfig.dump() << "'"; // typo in ansible-tor-bridge, see ansible-tor-bridge/defaults/main.yml. 
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

    if (m_ParsingResults == false && output.rfind("PLAY RECAP *", 0) == 0)
    {
        m_ParsingResults = true;
    }
    else if (m_ParsingResults)
    {
        Bridge* pBridge = GetBridgeFromOutput(output);
        if (pBridge != nullptr)
        {

            pBridge->SetState(GetSuccessFromOutput(output) ? "Deployed" : "Deployment failed");
        }
    }
    
}

Bridge* Deployment::GetBridgeFromOutput(const std::string& output) const
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

bool Deployment::GetSuccessFromOutput(const std::string& output) const
{
    return output.find("failed=0") != std::string::npos;
}

} // namespace Turbine
