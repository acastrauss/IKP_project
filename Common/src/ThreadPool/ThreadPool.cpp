#include "pch.h"
#include "ThreadPool/ThreadPool.h"
#include <Common/CommonMethods.h>
#include <iostream>

namespace Common {
	Common::ThreadPool::ThreadPool
		(DWORD(__stdcall* f)(LPVOID lpParam), const std::vector<LPVOID>& lpParams, USHORT poolSize)
		:m_Threads(), m_TakenThreads(0), initialized(true)
	{
		ASSERT(lpParams.size() == (size_t)poolSize);

		for (USHORT i = 0; i < poolSize; i++)
		{
			Common::ThreadInfo tInfo;

			tInfo.Handle = CreateThread(
				NULL, 0, f, lpParams[i], CREATE_SUSPENDED, &tInfo.Id
			);
		
			tInfo.Parameter = lpParams[i];
			tInfo.Taken = false;

			ASSERT(tInfo.Handle);

			m_Threads.push_back(tInfo);
		}

		initialized = true;

		InitializeConditionVariable(&cvThreads);
		InitializeCriticalSection(&csThreads);
	}

	Common::ThreadPool::ThreadPool()
		: m_Threads(), m_TakenThreads(0), initialized(false)
	{
		InitializeConditionVariable(&cvThreads);
		InitializeCriticalSection(&csThreads);
	}

	void Common::ThreadPool::AddThreadsAfterwards
		(DWORD(__stdcall* f)(LPVOID lpParam), std::vector<LPVOID> lpParams, USHORT poolSize)
	{
		if (!initialized) {
			EnterCriticalSection(&csThreads);

			ASSERT(lpParams.size() == (size_t)poolSize);

			for (USHORT i = 0; i < poolSize; i++)
			{
				Common::ThreadInfo tInfo;

				tInfo.Handle = CreateThread(
					NULL, 0, f, lpParams[i], CREATE_SUSPENDED, &tInfo.Id
				);

				tInfo.Parameter = lpParams[i];
				tInfo.Taken = false;

				ASSERT(tInfo.Handle);

				m_Threads.push_back(tInfo);
			}

			initialized = true;

			LeaveCriticalSection(&csThreads);
		}
	}

	Common::ThreadPool::~ThreadPool()
	{
		for (unsigned short i = 0; i < m_Threads.size(); i++)
		{
			SAFE_DELETE_HANDLE(m_Threads[i].Handle);
		}

		DeleteCriticalSection(&csThreads);
	}

	ThreadInfo* Common::ThreadPool::GetThreadNonBlocking()
	{
		Common::ThreadInfo* tInfo = NULL;

		EnterCriticalSection(&csThreads);

		if (m_TakenThreads != (USHORT)m_Threads.size()) {
		for (USHORT i = 0; i < (USHORT)m_Threads.size(); i++)
		{
			if (!m_Threads[i].Taken && !m_Threads[i].Finished) {
				m_Threads[i].Taken = true;
				m_TakenThreads++;
				tInfo = &m_Threads[i];
				break;
			}
		}
		}

		LeaveCriticalSection(&csThreads);

		return tInfo;
	}

	ThreadInfo* Common::ThreadPool::GetThreadBlocking()
	{
		EnterCriticalSection(&csThreads);

		while (
			m_TakenThreads == (USHORT)m_Threads.size()
			) {
			SleepConditionVariableCS(
				&cvThreads, &csThreads, INFINITE
			);
		}

		Common::ThreadInfo* tInfo = NULL;

		for (USHORT i = 0; i < (USHORT)m_Threads.size(); i++)
		{
			if (!m_Threads[i].Taken && !m_Threads[i].Finished) {
				m_Threads[i].Taken = true;
				m_TakenThreads++;
				tInfo = &m_Threads[i];
				break;
			}
		}
		LeaveCriticalSection(&csThreads);

		return tInfo;
	}

	void Common::ThreadPool::WaitForThreads()
	{
		for (USHORT i = 0; i < m_Threads.size(); i++)
		{
			WaitForSingleObject(m_Threads[i].Handle, INFINITE);
		}
	}
}