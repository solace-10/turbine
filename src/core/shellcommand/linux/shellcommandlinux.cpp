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

#include "core/platform.hpp"

#if defined(TARGET_PLATFORM_LINUX)

#include <sstream>

#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "core/shellcommand/linux/shellcommandlinux.hpp"
#include "log.h"

namespace Turbine 
{

ShellCommandLinux::ShellCommandLinux(const std::string& command, ShellCommandOnCompletionCallback completionCallback, ShellCommandOnOutputCallback stdOutCallback, ShellCommandOnOutputCallback stdErrCallback) :
ShellCommandImpl(command, completionCallback, stdOutCallback, stdErrCallback),
m_State(ShellCommand::State::PendingExecution),
m_CompletionCallback(completionCallback),
m_StdOutCallback(stdOutCallback),
m_StdErrCallback(stdErrCallback),
m_Command(command),
m_LineBufferStdOutIndex(0u),
m_LineBufferStdErrIndex(0u)
{

}

ShellCommandLinux::~ShellCommandLinux()
{
	if (m_State == ShellCommand::State::Running)
	{
		kill(m_Pid, SIGKILL);
	}
}

void ShellCommandLinux::Run()
{
	if (GetState() != ShellCommand::State::PendingExecution)
	{
		return;
	}

	if (pipe2(m_PipeStdOut, 0) < 0 || pipe2(m_PipeStdErr, 0) < 0)
	{
		Log::Error("Error creating pipe.");
		return;
	}

	m_Pid = fork();
	if (m_Pid == 0)
	{
		close(m_PipeStdOut[0]);
		close(m_PipeStdErr[0]);
		if (dup2(m_PipeStdOut[1], STDOUT_FILENO) == -1 || dup2(m_PipeStdErr[1], STDERR_FILENO) == -1)
		{
			exit(errno);
		}
		int r = execl("/bin/bash", "/bin/bash", "-c", m_Command.c_str(), nullptr);
		close(m_PipeStdOut[1]);
		close(m_PipeStdErr[1]);
		exit(r == -1 ? errno : 0);
	}
	else
	{
		close(m_PipeStdOut[1]);
		close(m_PipeStdErr[1]);
		m_State = ShellCommand::State::Running;
	}
}

void ShellCommandLinux::Update()
{
	if (GetState() == ShellCommand::State::Running)
	{
		ProcessPipe(m_PipeStdOut[0], m_LineBufferStdOut, m_LineBufferStdOutIndex, m_StdOutCallback);
		ProcessPipe(m_PipeStdErr[0], m_LineBufferStdErr, m_LineBufferStdErrIndex, m_StdErrCallback);

		int waitStatus;
		int result = waitpid(m_Pid, &waitStatus, WNOHANG);
		if (result == m_Pid)
		{
			int exitStatus = WEXITSTATUS(waitStatus);

			if (m_CompletionCallback != nullptr)
			{
				m_CompletionCallback(exitStatus);
			}

			m_State = (exitStatus == 0) ? ShellCommand::State::Completed : ShellCommand::State::FailedToRun;
			close(m_PipeStdOut[0]);
			close(m_PipeStdErr[0]);
		}
		else if (result == -1)
		{
			m_State = ShellCommand::State::FailedToRun;
			close(m_PipeStdOut[0]);
			close(m_PipeStdErr[0]);
			Log::Warning("Error during waidpid() for command '%s': %s.", m_Command.c_str(), strerror(errno));
		}
	}
}

ShellCommand::State ShellCommandLinux::GetState() const
{
	return m_State;
}

void ShellCommandLinux::ProcessPipe(int pipe, LineBufferType& lineBuffer, size_t& lineBufferIndex, ShellCommandOnOutputCallback onOutputCallback)
{
	if (onOutputCallback == nullptr)
	{
		return;
	}

	auto ProcessOutput = [&](bool forceFlush)
	{
		for (size_t i = lineBufferIndex; i < lineBuffer.size(); ++i)
		{
			if (lineBuffer[i] == '\n' || (forceFlush && i == lineBuffer.size() - 1))
			{
				std::string line = lineBuffer.substr(lineBufferIndex, i - lineBufferIndex + 1);
				lineBufferIndex = i + 1;
				onOutputCallback(line);
			}
		}
	};

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(pipe, &fds);

	const int cBufferSize = 1024; 
	char pBuffer[cBufferSize];
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int ret = select(pipe + 1, &fds, nullptr, nullptr, &timeout);
	if (ret == -1)
	{
		Log::Warning("Error during select() for command '%s': %d.", m_Command.c_str(), ret);
		m_State = ShellCommand::State::FailedToRun;
	}
	else if (ret > 0 && FD_ISSET(pipe, &fds))
	{
		ssize_t bytesRead = read(pipe, pBuffer, cBufferSize);
		if (bytesRead == 0)
		{
			ProcessOutput(true);
		}
		else if (bytesRead > 0)
		{
			for (size_t i = 0; i < bytesRead; ++i)
			{
				lineBuffer += pBuffer[i];
			}

			ProcessOutput(false);
		}
		else if (bytesRead == -1)
		{
			Log::Warning("Error during read() for command '%s': %s.", m_Command.c_str(), strerror(errno));
			m_State = ShellCommand::State::FailedToRun;
		}
	}
}

} // namespace Turbine

#endif // TARGET_PLATFORM_LINUX
