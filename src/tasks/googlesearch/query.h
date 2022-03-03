///*
//MIT License
//
//Copyright (c) 2022 Pedro Nunes
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//*/
//
//#pragma once
//
//#include <ctime>
//#include <string>
//#include <vector>
//
//namespace Turbine {
//namespace Tasks {
//
////////////////////////////////////////////////////////////////////////////
//// Query
////////////////////////////////////////////////////////////////////////////
//
//class Query
//{
//public:
//	Query() {}
//	Query(const std::string& query);
//
//	const std::string& Get() const;
//	const std::string& GetEncoded() const;
//
//private:
//	void EncodeQuery();
//	std::string m_Query;
//	std::string m_EncodedQuery;
//};
//
//
////////////////////////////////////////////////////////////////////////////
//// QueryState
////
//// As Google's API only returns 10 results at a time, we need to keep track of
//// how many results we have processed and keep querying for more results.
//// We also keep track of when the query has last been executed, so we aren't
//// repeating the same queries every time (this also helps with the API's rate
//// limiting of 1000 queries/day).
////////////////////////////////////////////////////////////////////////////
//
//class QueryState
//{
//public:
//	QueryState();
//
//	std::time_t GetLastExecution() const;
//	void SetLastExecution(std::time_t value);
//	int GetResultCount() const;
//	void SetResultCount(int value);
//	int GetCurrentStart() const;
//	void SetCurrentStart(int value);
//	bool IsCompleted() const;
//	bool IsValid() const;
//
//private:
//	std::time_t m_LastExecution;
//	int m_ResultCount;
//	int m_CurrentStart;
//};
//
//
////////////////////////////////////////////////////////////////////////////
//// QueryResult
////////////////////////////////////////////////////////////////////////////
//
//class QueryResult
//{
//public:
//	QueryResult(const std::string& url, const std::string& title);
//
//	const std::string& GetUrl() const;
//	const std::string& GetTitle() const;
//
//private:
//	std::string m_Url;
//	std::string m_Title;
//};
//
//using QueryResults = std::vector<QueryResult>;
//
//} // namespace Tasks
//} // namespace Turbine
