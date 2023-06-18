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

#include <string>
#include <vector>

#include "windows/window.h"

namespace Turbine
{

class LogWindow : public Window
{
public:
	LogWindow();
	virtual void Render() override;
	virtual void OnOpen() override;

	static int GetHeight();

    void AddOutput(const std::string& category, const std::string& output);

private:
	enum class OutputType
	{
		StdOut,
		StdErr
	};

	struct OutputLine
	{
		OutputType type;
		std::string timestamp;
		std::string content;
	};

	struct Category
	{
		std::string name;
		std::vector<OutputLine> lines;
	};
	std::vector<Category> m_Categories;
};

} // namespace Turbine
