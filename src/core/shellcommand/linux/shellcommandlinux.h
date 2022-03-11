#pragma once

#include <array>
#include <functional>
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
	ShellCommandLinux(const std::string& command, ShellCommandOnCompletionCallback completionCallback = nullptr, ShellCommandOnOutputCallback outputCallback = nullptr);
	~ShellCommandLinux();

	virtual void Run() override;
	virtual void Update() override;
	virtual ShellCommand::State GetState() const override;
	virtual const std::vector<std::string>& GetOutput() const override;

private:
	void RunInternal();
	void AddLineToOutput();
	ShellCommand::State m_State;
	ShellCommandOnCompletionCallback m_CompletionCallback;
	ShellCommandOnOutputCallback m_OutputCallback;
	std::string m_Command;
	std::array<char, 1024> m_LineBuffer;
	size_t m_LineBufferIndex;
	std::vector<std::string> m_Output;

	pid_t m_Pid;
	int m_Pipe[2];
};

} // namespace Turbine
