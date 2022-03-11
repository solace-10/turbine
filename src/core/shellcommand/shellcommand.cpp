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

#include "core/shellcommand/shellcommand.hpp"
#include "core/shellcommand/shellcommandimpl.hpp"
#include "core/platform.hpp"

#ifdef TARGET_PLATFORM_LINUX
#include "core/shellcommand/linux/shellcommandlinux.h"
#else
#include "core/shellcommand/null/shellcommandnull.h"
#endif

namespace Turbine 
{

ShellCommand::ShellCommand(const std::string& command, ShellCommandOnCompletionCallback completionCallback, ShellCommandOnOutputCallback outputCallback)
{
#ifdef TARGET_PLATFORM_LINUX
    m_pImpl = std::make_unique<ShellCommandLinux>(command, completionCallback, outputCallback);
#else
    m_pImpl = std::make_unique<ShellCommandNull(command, completionCallback, outputCallback);
#endif
}

ShellCommand::~ShellCommand()
{

}

void ShellCommand::Run()
{
    m_pImpl->Run();
}

void ShellCommand::Update()
{
    m_pImpl->Update();
}

ShellCommand::State ShellCommand::GetState() const
{
    return m_pImpl->GetState();
}

const std::vector<std::string>& ShellCommand::GetOutput() const
{
    return m_pImpl->GetOutput();
}

} // namespace Turbine