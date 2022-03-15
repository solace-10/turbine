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

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include "windows/deploymentwindow.hpp"

namespace Turbine
{

DeploymentWindow::DeploymentWindow()
{
}

void DeploymentWindow::Render()
{
	if (IsOpen() == false)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(650, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Deployments", &m_IsOpen))
	{
		ImGui::End();
		return;
	}

    ImGui::TextWrapped("%s", m_Output.c_str());

	ImGui::End();
}

void DeploymentWindow::OnOpen()
{

}

void DeploymentWindow::AddOutput(const std::string& output)
{
    m_Output += output;
}

} // namespace Turbine
