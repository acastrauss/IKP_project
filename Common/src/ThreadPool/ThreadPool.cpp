#include "pch.h"
#include "ThreadPool/ThreadPool.h"
#include <Common/CommonMethods.h>

namespace Common {
	Common::ThreadPool::ThreadPool
		(DWORD(__stdcall* f)(LPVOID lpParam), std::vector<LPVOID> lpParams, USHORT poolSize)
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

		m_TakenThreads = 0;
		
		InitializeConditionVariable(&cvThreads);
		InitializeCriticalSection(&csThreads);
	}

	Common::ThreadPool::~ThreadPool()
	{
		for (unsigned short i = 0; i < m_Threads.size(); i++)
		{
			SAFE_DELETE_HANDLE(m_Threads[i].Handle);
		}

		DeleteCriticalSection(&csThreads);
	}

	ThreadInfo Common::ThreadPool::GetThreadNonBlocking()
	{
		Common::ThreadInfo tInfo;

		EnterCriticalSection(&csThreads);

		if (m_TakenThreads == (USHORT)m_Threads.size()) {
			tInfo = ThreadInfo();
		}
		else {
			for (USHORT i = 0; i < (USHORT)m_Threads.size(); i++)
			{
				if (!m_Threads[i].Taken) {
					m_Threads[i].Taken = true;
					m_TakenThreads++;
					tInfo = m_Threads[i];
					break;
				}
			}
		}

		LeaveCriticalSection(&csThreads);

		return tInfo;
	}

	ThreadInfo Common::ThreadPool::GetThreadBlocking()
	{
		while (
			m_TakenThreads == (USHORT)m_Threads.size()
			) {
			SleepConditionVariableCS(
				&cvThreads, &csThreads, INFINITE
			);
		}

		EnterCriticalSection(&csThreads);

		Common::ThreadInfo tInfo;

		for (USHORT i = 0; i < (USHORT)m_Threads.size(); i++)
		{
			if (!m_Threads[i].Taken) {
				m_Threads[i].Taken = true;
				m_TakenThreads++;
				tInfo = m_Threads[i];
			}
		}
		LeaveCriticalSection(&csThreads);

		return tInfo;
	}

	void Common::ThreadPool::ReturnThreadToPool(DWORD tId)
	{
		EnterCriticalSection(&csThreads);

		for (USHORT i = 0; i < (USHORT)m_Threads.size(); i++)
		{
			if (m_Threads[i].Id == tId) {
				m_Threads[i].Taken = false;
				m_TakenThreads--;
				WakeConditionVariable(&cvThreads);
				break;
			}
		}

		LeaveCriticalSection(&csThreads);
	}
}