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

#include <filesystem>
#include <fstream>
#include <string>

#include "ext/json.h"
#include "database/database.h"
#include "tasks/searchtask.h"
#include "log.h"
#include "turbine.h"

namespace Turbine
{

SearchTask::SearchTask(const std::string& name, const std::filesystem::path& defaultQueriesFilePath) : 
Task(name),
m_DefaultQueriesFilePath(defaultQueriesFilePath),
m_QueriesAddedFromDatabase(false)
{

}

SearchTask::~SearchTask() 
{

}

void SearchTask::Update(float delta)
{
    Task::Update(delta);

	if (m_QueriesAddedFromDatabase == false)
	{
		AddQueriesFromDatabase();
		m_QueriesAddedFromDatabase = true;
	}
}

void SearchTask::Start()
{
    Task::Start();
}

void SearchTask::Stop()
{
    Task::Stop();
}

void SearchTask::OnDatabaseCreated(Database* pDatabase)
{
    const std::filesystem::path& path = GetDefaultQueriesFilePath();
    if (std::filesystem::exists(path))
    {
        using json = nlohmann::json;
		std::ifstream file(path);
		if ( file.is_open() )
		{
			json data;
			file >> data;
			file.close();

			if (data.is_null())
			{
				return;
			}

			if (data.is_array() == false)
			{
				Log::Error("Invalid format for '%s': root must be an array.", path.c_str());
				return;
			}

			for (json& entry : data)
			{
				if (entry.is_string())
				{
					AddQuery(entry.get<std::string>());
				}
			}
		}
    }
}

void SearchTask::AddQuery(const std::string& query)
{
	for (const auto& queryData : m_Queries)
	{
		if (queryData.query == query)
		{
			return;
		}
	}

	AddQueryToDatabase(query);
	AddQueryInternal(query);
}

void SearchTask::AddQueriesFromDatabase()
{
	Database* pDatabase = g_pTurbine->GetDatabase();
	PreparedStatement statement(pDatabase, "SELECT * FROM SearchQueries WHERE Engine IS ?1;", &SearchTask::DatabaseSelectQueriesCallback, this);
	statement.Bind(1, GetName());
	pDatabase->Execute(statement);
}

void SearchTask::AddQueryToDatabase(const std::string& query)
{
	Database* pDatabase = g_pTurbine->GetDatabase();
	PreparedStatement statement(pDatabase, "INSERT INTO SearchQueries VALUES(?1, ?2, ?3);");
	statement.Bind(1, GetName());
	statement.Bind(2, query);
	statement.Bind(3, "");
	pDatabase->Execute(statement);
}

void SearchTask::AddQueryInternal(const std::string& query)
{
	QueryData queryData;
	queryData.query = query;
	m_Queries.push_back(queryData);
}

void SearchTask::RemoveQuery(const std::string& query)
{
	SDL_assert(false); // Not implemented.
}

void SearchTask::OnResult(const std::string& result, const DatabaseTime& time)
{
	SDL_assert(false); // Not implemented.
}

void SearchTask::DatabaseSelectQueriesCallback(const QueryResult& result, void* pData)
{
	for (auto& row : result.Get())
	{
		static const int numCells = 3;
		if (numCells != row.size())
		{
			Log::Error("Invalid number of rows returned from query in DatabaseSelectQueriesCallback(). Expected %d, got %d.", numCells, row.size());
			continue;
		}

		SearchTask* pTask = reinterpret_cast<SearchTask*>(pData);
		QueryData queryData;
		queryData.query = row[1]->GetString();
		std::string date = row[2]->GetString();
		pTask->m_Queries.push_back(queryData);
	}
}

} // namespace Watcher
