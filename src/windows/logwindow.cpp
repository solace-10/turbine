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

#include "windows/logwindow.hpp"
#include "fonts.h"

#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>

namespace Turbine
{

LogWindow::LogWindow()
{
}

void LogWindow::Render()
{
	if (IsOpen() == false)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(650, 400), ImGuiCond_FirstUseEver);
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
	if (!ImGui::Begin("Logs", &m_IsOpen))
	{
		ImGui::PopStyleColor();
		ImGui::End();
		return;
	}
	ImGui::PopStyleColor();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4)); // Not working correctly in ImGui 1.88 WIP.
	ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("LogTabs", tabBarFlags))
	{
		for (Category& category : m_Categories)
		{
			if (ImGui::BeginTabItem(category.name.c_str()))
			{
				if (ImGui::BeginChild("LogContents"))
				{
					ImGui::PushFont(Fonts::GetFont(FontId::Inconsolata18));
					for (auto& line : category.lines)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 225, 255));
						ImGui::TextUnformatted(line.timestamp.c_str());
						ImGui::SameLine();
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(75, 171, 225, 255));
						ImGui::TextWrapped("%s", line.content.c_str());
						ImGui::PopStyleColor(2);
					}
					ImGui::PopFont();
					ImGui::EndChild();
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::PopStyleVar();

	ImGui::End();
}

void LogWindow::OnOpen()
{

}

void LogWindow::AddOutput(const std::string& categoryName, const std::string& output)
{
	Category* pCategory = nullptr;
	for (auto& category : m_Categories)
	{
		if (category.name == categoryName)
		{
			pCategory = &category;
			break;
		}
	}

	if (pCategory == nullptr)
	{
		Category category;
		category.name = categoryName;
		m_Categories.push_back(category);
		pCategory = &m_Categories.back();
	}

    std::stringstream timestamp;
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	tm* timeinfo = std::localtime(&time);
	timestamp << std::put_time(timeinfo, "%H:%M:%S ");

	OutputLine line;
	line.type = OutputType::StdOut;
	line.timestamp = timestamp.str();
	line.content = output;
	pCategory->lines.push_back(std::move(line));
}

} // namespace Turbine
