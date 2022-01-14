#include "ThreadPool/ThreadInfo.h"
#include "pch.h"
#include "ThreadPool/ThreadInfo.h"

namespace Common {
	Common::ThreadInfo::ThreadInfo()
	{
		Handle = NULL;
		Id = -1;
		Taken = false;
		Finished = false;
		Parameter = NULL;
	}

	Common::ThreadInfo::ThreadInfo(HANDLE handle, DWORD id, LPVOID parameter, bool taken, bool finished)
	{
		Handle = handle;
		Id = id;
		Parameter = parameter;
		Taken = taken;
		Finished = finished;
	}

	Common::ThreadInfo::ThreadInfo(const ThreadInfo& rhs)
	{
		Handle = rhs.Handle;
		Id = rhs.Id;
		Parameter = rhs.Parameter;
		Taken = rhs.Taken;
		Finished = rhs.Finished;
	}

	ThreadInfo& Common::ThreadInfo::operator=(const ThreadInfo& rhs)
	{
		Handle = rhs.Handle;
		Id = rhs.Id;
		Parameter = rhs.Parameter;
		Taken = rhs.Taken;
		Finished = rhs.Finished;
		return *this;
	}
}