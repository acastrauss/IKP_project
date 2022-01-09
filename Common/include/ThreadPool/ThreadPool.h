#pragma once

#include "ThreadPool/ThreadInfo.h"
#include <vector>


namespace Common {
	

	class ThreadPool
	{
	private:
		std::vector<ThreadInfo> m_Threads;
		unsigned short m_TakenThreads = 0;

		CONDITION_VARIABLE cvThreads;
		CRITICAL_SECTION csThreads;
	public:

		explicit ThreadPool(
			DWORD(WINAPI* f)(LPVOID lpParam),
			LPVOID lpParam,
			USHORT poolSize
		);

		~ThreadPool();

		ThreadPool() = delete;

		ThreadPool(
			const ThreadPool& rhs
		) = delete;

		ThreadPool& operator=(
			const ThreadPool& rhs
		) = delete;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>
		/// Returns: ThreadInfo with Taken=true if there is available thread,
		/// Thread info with Taken=false and Handle=NULL othrewise
		/// </returns>
		ThreadInfo GetThreadNonBlocking();
		
		/// <summary>
		/// 
		/// </summary>
		/// <returns>
		/// Returns: handle for thread if there is one available,
		/// blocks until there is one available otherwise
		/// </returns>
		ThreadInfo GetThreadBlocking();

		void ReturnThreadToPool(const ThreadInfo& threadInfo);
	};
}


