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

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>

#include "query.h"

namespace Turbine {
namespace Tasks {

//////////////////////////////////////////////////////////////////////////
// Query
//////////////////////////////////////////////////////////////////////////

Query::Query(const std::string& query):
m_Query(query)
{
	EncodeQuery();
}

void Query::EncodeQuery()
{
	std::ostringstream escaped;
	escaped.fill('0');
	escaped << std::hex;

	for (std::string::value_type c : m_Query) 
	{
		// Keep alphanumeric and other accepted characters intact.
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') 
		{
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded.
		escaped << std::uppercase;
		escaped << '%' << std::setw(2) << int((unsigned char)c);
		escaped << std::nouppercase;
	}

	m_EncodedQuery = escaped.str();
}

const std::string& Query::Get() const
{
	return m_Query;
}

const std::string& Query::GetEncoded() const
{
	return m_EncodedQuery;
}


//////////////////////////////////////////////////////////////////////////
// QueryState
//////////////////////////////////////////////////////////////////////////

QueryState::QueryState()
{
	m_LastExecution = std::time(0);
	m_ResultCount = -1;
	m_CurrentStart = -1;
}

std::time_t QueryState::GetLastExecution() const
{
	return m_LastExecution;
}

void QueryState::SetLastExecution(std::time_t value)
{
	m_LastExecution = value;
}

int QueryState::GetResultCount() const
{
	return m_ResultCount;
}

void QueryState::SetResultCount(int value)
{
	// A limitation of Google's Custom Search Engine is that it will only allow the first
	// 100 results to be queried. Going above this will generate an error.
	if (value > 100)
	{
		m_ResultCount = 100;
	}
	else
	{
		m_ResultCount = value;
	}
}

int QueryState::GetCurrentStart() const
{
	return m_CurrentStart;
}

void QueryState::SetCurrentStart(int value)
{
	m_CurrentStart = value;
}

bool QueryState::IsCompleted() const
{
	return IsValid() && GetCurrentStart() >= GetResultCount();
}

bool QueryState::IsValid() const
{
	return GetResultCount() >= 0 && GetCurrentStart() >= 0;
}


//////////////////////////////////////////////////////////////////////////
// QueryResult
//////////////////////////////////////////////////////////////////////////

QueryResult::QueryResult(const std::string& url, const std::string& title)
{
	m_Url = url;
	m_Title = title;
}

const std::string& QueryResult::GetUrl() const
{
	return m_Url;
}

const std::string& QueryResult::GetTitle() const
{
	return m_Title;
}

} // namespace Tasks
} // namespace Watcher
