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

#include "bridge/bridge.fwd.hpp"

#include <json.hpp>

#include <string>
#include <vector>

namespace Turbine
{

class AnsibleCommand
{
public:
    AnsibleCommand(const std::string& inventoryName);
    ~AnsibleCommand();

    virtual void OnDeploymentCommandStandardOutput(const std::string& output);
    virtual void OnDeploymentCommandErrorOutput(const std::string& output);
    virtual void OnSuccess(Bridge* pBridge);
    virtual void OnUnreachable(Bridge* pBridge, const std::string& error);
    virtual void OnFailed(Bridge* pBridge, const std::string& error);

protected:
    const std::string& GetInventoryName() const;
    void GenerateInventory();
    Bridge* GetBridgeFromOutput(const std::string& output) const;
    void HandleGatheringFacts(const std::string& output);
    void HandlePlayRecap(const std::string& output);

    struct GatheringFactsResult
    {
        Bridge* pBridge;
        bool ok;
        nlohmann::json details;
    };
    std::vector<GatheringFactsResult> m_GatheringFactsResults;
    bool m_GatheringFacts;
    bool m_PlayRecap;

private:
    std::string m_InventoryName;
};

} // namespace Turbine
