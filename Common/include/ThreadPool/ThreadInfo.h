#pragma once
#include <WinSock2.h>

namespace Common {
	struct ThreadInfo {
		HANDLE Handle;
		DWORD Id;
		LPVOID Parameter; // so parameters can be edited in runtime
		bool Taken = false;
		bool Finished = false;

		ThreadInfo();
		ThreadInfo(
			HANDLE handle,
			DWORD id,
			LPVOID parameter,
			bool taken = false,
			bool finished = false
		);

		ThreadInfo(
			const ThreadInfo& rhs
		);

		ThreadInfo& operator= (
			const ThreadInfo& rhs
		);
	};
}