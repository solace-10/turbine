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

#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <list>
#include <stdarg.h>
#include <string>
#include <vector>

#include <SDL.h>

namespace Turbine
{

class ILogTarget;

//////////////////////////////////////////////////////////////////////////
// Log
// Contains any number of ILogTargets, which are responsible for actually 
// logging the message in various ways. 
// This class is thread safe.
//////////////////////////////////////////////////////////////////////////

using LogTargetSharedPtr = std::shared_ptr< ILogTarget >;
static const size_t sLogBufferSize = 20480u;

enum class LogLevel
{
	Info,
	Warning,
	Error
};

class Log
{
public:
    static void Info( const char* pFormat, ... );
    static void Warning( const char* pFormat, ... );
    static void Error( const char* pFormat, ... );

    static void AddLogTarget( LogTargetSharedPtr pLogTarget );
    static void RemoveLogTarget( LogTargetSharedPtr pLogTarget );

private:
	using LogTargetList = std::list< LogTargetSharedPtr >;

    static void LogInternal( const std::string& text, LogLevel level );

    static std::mutex m_Mutex;
    static LogTargetList m_Targets;
    static std::array< char, sLogBufferSize > m_Buffer;
    static std::array< char, sLogBufferSize > m_VABuffer;
};


//////////////////////////////////////////////////////////////////////////
// ILogTarget
// Any ILogTarget must implement Log().
//////////////////////////////////////////////////////////////////////////

class ILogTarget
{
public:
	virtual ~ILogTarget() {}
    virtual void Log( const std::string& text, LogLevel level ) = 0;
};


//////////////////////////////////////////////////////////////////////////
// FileLogger
// Dumps the logging into file given in "filename". It is flushed
// after every entry.
//////////////////////////////////////////////////////////////////////////

class FileLogger : public ILogTarget
{
public:
    FileLogger( const char* pFilename );
    virtual ~FileLogger() override;
    virtual void Log( const std::string& text, LogLevel type ) override;

private:
    std::ofstream m_File;
};


//////////////////////////////////////////////////////////////////////////
// MessageBoxLogger
// Creates a message box whenever the log message is above LogLevel::Info.
//////////////////////////////////////////////////////////////////////////

class MessageBoxLogger : public ILogTarget
{
public:
	virtual void Log( const std::string& text, LogLevel type ) override;
};


//////////////////////////////////////////////////////////////////////////
// VisualStudioLogger
// Windows only - all the output from the logger goes to the
// Visual Studio output window.
//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
class VisualStudioLogger : public ILogTarget
{
public:
	virtual void Log( const std::string& text, LogLevel type ) override;
};
#endif


//////////////////////////////////////////////////////////////////////////
// NotificationLogger
//////////////////////////////////////////////////////////////////////////

class NotificationLogger : public ILogTarget
{
public:
    NotificationLogger(SDL_Window* pWindow);

	virtual void Log(const std::string& text, LogLevel type) override;

    void Update(float delta);
    void Render();

private:
	struct Entry
    {
        Entry(const std::string& text, LogLevel level)
        {
            m_Text = text;
            m_Level = level;
            m_Timer = 10.0f;
        }

        std::string m_Text;
        LogLevel m_Level;
        float m_Timer;
    };
    using Entries = std::list<Entry>;
    Entries m_Entries;
    SDL_Window* m_pWindow;
};

} // namespace Turbine
