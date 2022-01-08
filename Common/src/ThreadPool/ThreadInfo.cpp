#include "ThreadPool/ThreadInfo.h"

namespace Common {
	Common::ThreadInfo::ThreadInfo()
	{
		Handle = NULL;
		Id = -1;
		Taken = false;
	}

	Common::ThreadInfo::ThreadInfo(HANDLE handle, DWORD id, bool taken)
	{
		Handle = handle;
		Id = id;
		Taken = taken;
	}

	Common::ThreadInfo::ThreadInfo(const ThreadInfo& rhs)
	{
		Handle = rhs.Handle;
		Id = rhs.Id;
		Taken = rhs.Taken;
	}

	ThreadInfo& Common::ThreadInfo::operator=(const ThreadInfo& rhs)
	{
		Handle = rhs.Handle;
		Id = rhs.Id;
		Taken = rhs.Taken;
		return *this;
	}
}