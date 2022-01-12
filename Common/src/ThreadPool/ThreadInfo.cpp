#include "ThreadPool/ThreadInfo.h"
#include "pch.h"
#include "ThreadPool/ThreadInfo.h"

namespace Common {
	Common::ThreadInfo::ThreadInfo()
	{
		Handle = NULL;
		Id = -1;
		Taken = false;
		Parameter = NULL;
	}

	Common::ThreadInfo::ThreadInfo(HANDLE handle, DWORD id, LPVOID parameter, bool taken)
	{
		Handle = handle;
		Id = id;
		Parameter = parameter;
		Taken = taken;
	}

	Common::ThreadInfo::ThreadInfo(const ThreadInfo& rhs)
	{
		Handle = rhs.Handle;
		Id = rhs.Id;
		Parameter = rhs.Parameter;
		Taken = rhs.Taken;
	}

	ThreadInfo& Common::ThreadInfo::operator=(const ThreadInfo& rhs)
	{
		Handle = rhs.Handle;
		Id = rhs.Id;
		Parameter = rhs.Parameter;
		Taken = rhs.Taken;
		return *this;
	}
}