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

#include <string>

namespace Turbine
{

class Database;

///////////////////////////////////////////////////////////////////////////////
// Task
// A background task, performing work over time without requiring interaction
// by the user. These are used to discover new cameras in various ways, as well
// as where they're located, etc.
///////////////////////////////////////////////////////////////////////////////

class Task
{
public:
	enum class State
	{
		Disabled,
		Idle,
		Running,
		Error
	};

	Task(const std::string& name);
	virtual ~Task();
	virtual void Update(float delta);

	virtual void Start();
	virtual void Stop();
	virtual void OnDatabaseCreated(Database* pDatabase);

	void Enable();
	void Disable();
	void Render();

	const std::string& GetName() const;

protected:
	State GetState() const;
	void SetState(State state);
	void SetErrorString(const std::string& error);

private:
	std::string m_Name;
	State m_State;
	std::string m_Error;
	float m_SpinnerTimer;
};

inline const std::string& Task::GetName() const
{
	return m_Name;
}

inline Task::State Task::GetState() const
{
	return m_State;
}

inline void Task::SetState(State state)
{
	m_State = state;
}

inline void Task::SetErrorString(const std::string& error)
{
	m_Error = error;
}

} // namespace Turbine
