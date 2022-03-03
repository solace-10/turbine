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

#include <algorithm>

#ifdef _WIN32
#include "windows.h"
#endif

#include "log.h"

namespace Turbine
{

//////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////

std::mutex Log::m_Mutex;
Log::LogTargetList Log::m_Targets;
std::array< char, sLogBufferSize > Log::m_Buffer;
std::array< char, sLogBufferSize > Log::m_VABuffer;

void Log::AddLogTarget( LogTargetSharedPtr pLogTarget )
{
	std::lock_guard< std::mutex > lock( m_Mutex );
	m_Targets.push_back( pLogTarget );
}

void Log::RemoveLogTarget( LogTargetSharedPtr pLogTarget )
{
	std::lock_guard< std::mutex > lock( m_Mutex );
	m_Targets.remove( pLogTarget );
}

// Internal logging function. Should be called by one of the public functions (LogInfo / LogWarning / LogError).
// Assumes that m_Mutex is locked at this stage.
void Log::LogInternal( const std::string& text, LogLevel level )
{
	std::string prefix;
	if ( level == LogLevel::Info ) prefix = "INFO: ";
	else if ( level == LogLevel::Warning ) prefix = "WARNING: ";
	else if ( level == LogLevel::Error ) prefix = "ERROR: ";

    snprintf( m_Buffer.data(), m_Buffer.size(), "%s%s\n", prefix.c_str(), text.c_str() );

    for ( auto& pTarget : m_Targets )
    {
		pTarget->Log( m_Buffer.data(), level );
    }
}

void Log::Info( const char* format, ... )
{
	std::lock_guard< std::mutex > lock( m_Mutex );

    va_list args;
    va_start( args, format );
	vsnprintf( m_VABuffer.data(), m_VABuffer.size(), format, args );
    LogInternal( m_VABuffer.data(), LogLevel::Info );
    va_end( args );
}

void Log::Warning( const char* format, ... )
{
	std::lock_guard< std::mutex > lock( m_Mutex );

    va_list args;
    va_start( args, format );
    vsnprintf( m_VABuffer.data(), m_VABuffer.size(), format, args );
    LogInternal( m_VABuffer.data(), LogLevel::Warning );
    va_end( args );
}

void Log::Error( const char* format, ... )
{
	std::lock_guard< std::mutex > lock( m_Mutex );

    va_list args;
    va_start( args, format );
    vsnprintf( m_VABuffer.data(), m_VABuffer.size(), format, args );
    LogInternal( m_VABuffer.data(), LogLevel::Error );
    va_end( args );

#ifdef _WIN32
	__debugbreak();
#else
    asm("int $3");
#endif
}


//////////////////////////////////////////////////////////////////////////
// FileLogger
//////////////////////////////////////////////////////////////////////////

FileLogger::FileLogger( const char* pFilename )
{
    m_File.open( pFilename, std::fstream::out | std::fstream::trunc );
}

FileLogger::~FileLogger()
{
    if ( m_File.is_open() )
    {
        m_File.close();
    }
}

void FileLogger::Log( const std::string& text, LogLevel level )
{
    if ( !m_File.is_open() )
    {
        return;
    }

    m_File.write( text.c_str(), text.size() );
    m_File.flush();
}


//////////////////////////////////////////////////////////////////////////
// MessageBoxLogger
//////////////////////////////////////////////////////////////////////////

//void MessageBoxLogger::Log( char* pText, LogMessageType type )
//{
//    if ( type == LOG_WARNING )
//    {
//        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_WARNING, "Warning", pText, nullptr );
//    }
//    else if ( type == LOG_ERROR )
//    {
//        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", pText, nullptr );
//    }
//}


//////////////////////////////////////////////////////////////////////////
// VisualStudioLogger
//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
void VisualStudioLogger::Log( const std::string& text, LogLevel level )
{
    OutputDebugStringA( text.c_str() );
}
#endif

} // namespace Watcher
