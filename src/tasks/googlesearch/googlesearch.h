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

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include "ext/json.h"
#include "tasks/searchtask.h"
#include "query.h"

using json = nlohmann::json;

namespace Turbine
{

namespace Tasks
{

class GoogleSearch : public SearchTask
{
public:
	GoogleSearch();
	virtual ~GoogleSearch() override;
    virtual void Update(float delta) override;

	virtual void Start() override;
	virtual void Stop() override;

	void ShowQueriesUI(bool* pOpen);

private:
	static void ThreadMain(GoogleSearch* pGoogleSearch);
	static std::string FilterCurlData(const std::string& data);
	void ExecuteQuery(const std::string& query);
	static bool ExtractStartIndex(const json& data, int& result);
	static bool ExtractTotalResults(const json& data, int& result);
	static bool ExtractResults(const json& data, QueryResults& results);
	static void ProcessResults(GoogleSearch* pGoogleSearch, const QueryData& queryData);
	static bool FilterResult(const QueryData& queryData, const QueryResult& result);
	bool IsRunning() const;

	std::vector<SearchTask::QueryData> FindPendingQueries() const;

	std::thread m_QueryThread;
	std::atomic_bool m_QueryThreadActive;
	std::atomic_bool m_QueryThreadStopFlag;
	bool m_ShowResultsUI;
	std::string m_CurlData;
	std::mutex m_QueryDatumMutex;
	bool m_IsRunning;
};

} // namespace Tasks
} // namespace Watcher
