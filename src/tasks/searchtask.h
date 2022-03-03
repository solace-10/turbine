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

#include <filesystem>
#include <vector>

#include "database/databasetime.h"
#include "database/queryresult.h"
#include "tasks/task.h"

namespace Turbine
{

///////////////////////////////////////////////////////////////////////////////
// SearchTask
// A task intended to be used for searching for cameras, using a search
// engine and linked to a database.
///////////////////////////////////////////////////////////////////////////////

class SearchTask : public Task
{
public:
	SearchTask(const std::string& name, const std::filesystem::path& defaultQueriesFilePath);
	virtual ~SearchTask() override;
	virtual void Update(float delta) override;

	virtual void Start() override;
	virtual void Stop() override;
	virtual void OnDatabaseCreated(Database* pDatabase) override;

	void AddQuery(const std::string& query);
	void RemoveQuery(const std::string& query);

	virtual void OnResult(const std::string& result, const DatabaseTime& time);

protected:
	const std::filesystem::path& GetDefaultQueriesFilePath() const;
	void AddQueriesFromDatabase();
	void AddQueryToDatabase(const std::string& query);
	void AddQueryInternal(const std::string& query);
	static void DatabaseSelectQueriesCallback(const QueryResult& result, void* pData);

	struct QueryData
	{
		std::string query;
		DatabaseTime time;
	};
	std::vector<QueryData> m_Queries;
	bool m_QueriesAddedFromDatabase;

private:
	std::filesystem::path m_DefaultQueriesFilePath;
};

inline const std::filesystem::path& SearchTask::GetDefaultQueriesFilePath() const
{
	return m_DefaultQueriesFilePath;
}

} // namespace Watcher
