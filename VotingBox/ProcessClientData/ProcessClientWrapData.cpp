#include "ProcessClientWrapData.h"

ProcessClientWrapData::ProcessClientWrapData
	(SOCKET acceptedSocket, DWORD threadId, Common::ThreadPool* const threadPool, AcceptedSocketPool* const socketPool)
	:
	AcceptedSocket(acceptedSocket),
	ThreadId(threadId),
	ThreadPool(threadPool),
	SocketPool(socketPool)
{}

ProcessClientWrapData::ProcessClientWrapData(const ProcessClientWrapData& ref)
	:
	AcceptedSocket(ref.AcceptedSocket),
	ThreadId(ref.ThreadId),
	ThreadPool(ref.ThreadPool),
	SocketPool(ref.SocketPool)
{}