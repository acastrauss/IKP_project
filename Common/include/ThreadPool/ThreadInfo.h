#pragma once
#include "Windows.h"

namespace Common {
	struct ThreadInfo {
		HANDLE Handle;
		DWORD Id;
		bool Taken = false;

		ThreadInfo();
		ThreadInfo(
			HANDLE handle,
			DWORD id,
			bool taken = false
		);

		ThreadInfo(
			const ThreadInfo& rhs
		);

		ThreadInfo& operator= (
			const ThreadInfo& rhs
		);
	};
}