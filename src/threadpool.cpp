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

#include "threadpool.h"

ThreadPool::ThreadPool(int numThreads) :
m_Stop(false)
{
	for (int i = 0; i < numThreads; ++i)
	{
		m_Threads.emplace_back(&ThreadMain, this);
	}
}

ThreadPool::~ThreadPool()
{
	m_Stop = true;

	for (auto& thread : m_Threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
}

void ThreadPool::Queue(Job job)
{
	std::lock_guard<std::mutex> lock(m_JobMutex);
	m_Jobs.push_back(job);
	m_Condition.notify_one();
}

void ThreadPool::ThreadMain(ThreadPool* pThreadPool)
{
	while (pThreadPool->m_Stop == false)
	{
		Job job;
		{
			std::unique_lock<std::mutex> lock(pThreadPool->m_JobMutex);
			pThreadPool->m_Condition.wait(lock, [pThreadPool]() { return !pThreadPool->m_Jobs.empty(); });
			job = pThreadPool->m_Jobs.front();
			pThreadPool->m_Jobs.pop_front();
		}
		job();
	}
}