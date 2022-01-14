#pragma once

#include "ThreadPool/ThreadInfo.h"
#include <vector>

namespace Common {
	
	class ThreadPool
	{
	private:
		std::vector<ThreadInfo> m_Threads;
		unsigned short m_TakenThreads;
		bool initialized;


		CONDITION_VARIABLE cvThreads;
		CRITICAL_SECTION csThreads;
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="f">:Function for threads</param>
		/// <param name="lpParams">:Vector of parameters for each thread (size must be equal to pool size)</param>
		/// <param name="poolSize"></param>
		explicit ThreadPool(
			DWORD(WINAPI* f)(LPVOID lpParam),
			const std::vector<LPVOID>& lpParams,
			USHORT poolSize
		);

		~ThreadPool();

		ThreadPool();

		/// <summary>
		/// This method should be used only when pool is created with empty constructor,
		/// and it should be called only once
		/// </summary>
		/// <param name="rhs"></param>
		void AddThreadsAfterwards(
			DWORD(WINAPI* f)(LPVOID lpParam),
			std::vector<LPVOID> lpParams,
			USHORT poolSize
		);

		ThreadPool(
			const ThreadPool& rhs
		) = delete;

		ThreadPool& operator=(
			const ThreadPool& rhs
		) = delete;

		/// <summary>
		/// </summary>
		/// <returns>
		/// Returns: ThreadInfo with Taken=true if there is available thread,
		/// Thread info with Taken=false and Handle=NULL othrewise
		/// </returns>
		ThreadInfo* GetThreadNonBlocking();
		
		/// <summary>
		/// </summary>
		/// <returns>
		/// Returns: handle for thread if there is one available,
		/// blocks until there is one available otherwise
		/// </returns>
		ThreadInfo* GetThreadBlocking();

		void ReturnThreadToPool(DWORD tId);

		void WaitForThreads();
	};
}


