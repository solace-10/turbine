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

#include "core/platform.hpp"

#if defined(TARGET_PLATFORM_LINUX)

#include <array>
#include <memory>
#include <string>

#include <sys/types.h>
#include <unistd.h>

#include "core/shellcommand/shellcommand.hpp"
#include "core/shellcommand/shellcommandimpl.hpp"

namespace Turbine 
{

class ShellCommandLinux : public ShellCommandImpl
{
public:
	ShellCommandLinux(const std::string& command, ShellCommandOnCompletionCallback completionCallback = nullptr, ShellCommandOnOutputCallback stdOutCallback = nullptr, ShellCommandOnOutputCallback stdErrCallback = nullptr);
	~ShellCommandLinux();

	virtual void Run() override;
	virtual void Update() override;
	virtual ShellCommand::State GetState() const override;
	virtual const std::vector<std::string>& GetOutput() const override;

private:
	using LineBufferType = std::array<char, 1024>;

	void ProcessPipe(int pipe, LineBufferType& lineBuffer, size_t& lineBufferIndex, ShellCommandOnOutputCallback outputCallback);
	ShellCommand::State m_State;
	ShellCommandOnCompletionCallback m_CompletionCallback;
	ShellCommandOnOutputCallback m_StdOutCallback;
	ShellCommandOnOutputCallback m_StdErrCallback;
	std::string m_Command;
	LineBufferType m_LineBufferStdOut;
	size_t m_LineBufferStdOutIndex;
	LineBufferType m_LineBufferStdErr;
	size_t m_LineBufferStdErrIndex;
	std::vector<std::string> m_Output;

	pid_t m_Pid;
	int m_PipeStdOut[2];
	int m_PipeStdErr[2];
};

} // namespace Turbine

#endif // TARGET_PLATFORM_LINUX
