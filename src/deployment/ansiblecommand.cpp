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
#include "deployment/ansiblecommand.h"
#include "providers/provider.h"
#include "settings.h"
#include "turbine.h"

namespace Turbine
{

AnsibleCommand::AnsibleCommand()
{

}
    
AnsibleCommand::~AnsibleCommand()
{

}

void AnsibleCommand::GenerateInventory()
{
    Settings* pSettings = g_pTurbine->GetSettings();
    std::filesystem::path storagePath = pSettings->GetStoragePath();

	std::filesystem::path filePath = storagePath / "inventory";
	std::ofstream file(filePath, std::ofstream::out | std::ofstream::trunc);
    if (file.good())
    {
        for (int i = 0; i < 2; ++i)
        {
            std::string state = (i == 0) ? "Deployment pending" : "Deployed";
            std::string group = (i == 0) ? "DeploymentPending" : "Deployed";

            file << "[" << group << "]\n";

            for (auto& pBridge : g_pTurbine->GetBridges())
            {
                if (pBridge->GetState() != state)
                {
                    continue;
                }

                if (pBridge->GetIPv4().empty() && pBridge->GetIPv6().empty())
                {
                    continue;
                }

                const json additionalConfig = 
                {
                    {{"name", "PublishServerDescriptor"}, {"value", "bridge"}},
                    {{"name", "Log"}, {"value", "notice file /var/log/tor/notices.log"}},
                    {{"name", "CellStatistics"}, {"value", "1"}},
                    {{"name", "DirReqStatistics"}, {"value", "1"}},
                    {{"name", "EntryStatistics"}, {"value", "1"}},
                    {{"name", "ExitPortStatistics"}, {"value", "1"}},
                    {{"name", "ConnDirectionStatistics"}, {"value", "1"}},
                    {{"name", "ExtraInfoStatistics"}, {"value", "1"}},
                    {{"name", "BridgeRecordUsageByCountry"}, {"value", "1"}},
                };

                const std::string& ip = pBridge->GetIPv4().size() > 0 ? pBridge->GetIPv4() : pBridge->GetIPv6();
                file << ip;
                file << " nickname=" << pBridge->GetName();
                file << " ansible_user=root";
                file << " public_ip=" << ip;
                file << " email_address=" << pSettings->GetContactEmail();
                file << " tor_port=" << pBridge->GetORPort();
                file << " obfs4_port=" << pBridge->GetExtPort();
                file << " additionnal_torrc_config='" << additionalConfig.dump() << "'"; // typo in ansible-tor-bridge, see ansible-tor-bridge/defaults/main.yml. 
                file << "\n";
            }

            file << "\n";
        }
        file.close();
    }
}

} // namespace Turbine
