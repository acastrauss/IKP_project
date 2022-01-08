#include "pch.h"
#include "ThreadPool/ThreadPool.h"
#include <Common/CommonMethods.h>

namespace Common {
	Common::ThreadPool::ThreadPool
		(DWORD(__stdcall* f)(LPVOID lpParam), LPVOID lpParam, USHORT poolSize)
	{
		for (USHORT i = 0; i < poolSize; i++)
		{
			Common::ThreadInfo tInfo;

			tInfo.Handle = CreateThread(
				NULL, 0, f, lpParam, CREATE_SUSPENDED, &tInfo.Id
			);
		
			ASSERT(tInfo.Handle);

			m_Threads.push_back(tInfo);
		}

		m_TakenThreads = 0;
	
		semaphoreHandle = CreateSemaphore(
			NULL, 1, 1, NULL
		);
	}

	Common::ThreadPool::~ThreadPool()
	{
		for (unsigned short i = 0; i < m_Threads.size(); i++)
		{
			SAFE_DELETE_HANDLE(m_Threads[i].Handle);
		}

		SAFE_DELETE_HANDLE(semaphoreHandle);
	}

	ThreadInfo Common::ThreadPool::GetThreadNonBlocking()
	{
		if (m_TakenThreads == (USHORT)m_Threads.size()) {
			return ThreadInfo();
		}
		else {
			for (USHORT i = 0; i < (USHORT)m_Threads.size(); i++)
			{
				if (!m_Threads[i].Taken) {
					m_Threads[i].Taken = true;
					m_TakenThreads++;
					return m_Threads[i];
				}
			}

			return ThreadInfo();
		}
	}

	ThreadInfo Common::ThreadPool::GetThreadBlocking()
	{
		if (
			m_TakenThreads == (USHORT)m_Threads.size()
			) {
			WaitForSingleObject(
				semaphoreHandle, INFINITE
			);
		}

		Common::ThreadInfo tInfo;

		for (USHORT i = 0; i < (USHORT)m_Threads.size(); i++)
		{
			if (!m_Threads[i].Taken) {
				tInfo = m_Threads[i];
				m_TakenThreads++;
			}
		}

		return tInfo;
	}

	void Common::ThreadPool::ReturnThreadToPool(const ThreadInfo& threadInfo)
	{
		for (USHORT i = 0; i < (USHORT)m_Threads.size(); i++)
		{
			if (m_Threads[i].Id == threadInfo.Id) {
				m_Threads[i].Taken = false;
				m_TakenThreads--;
			
				ReleaseSemaphore(
					semaphoreHandle, 1, NULL
				);
			}
		}
	}
}