#include "AcceptedSocketPool.h"

AcceptedSocketPool::AcceptedSocketPool(USHORT socketNum):
	m_Sockets(),
	m_TakenSockets(0)
{
	InitializeCriticalSection(&csSockets);
	InitializeConditionVariable(&cvSockets);

	for (USHORT i = 0; i < socketNum; i++)
	{
		m_Sockets.push_back(INVALID_SOCKET);
	}
}

AcceptedSocketPool::~AcceptedSocketPool()
{
	DeleteCriticalSection(&csSockets);
}

SOCKET AcceptedSocketPool::AssignNewSocket(SOCKET socket)
{
	if (
		m_TakenSockets == m_Sockets.size()
		) {
		SleepConditionVariableCS(&cvSockets, &csSockets, INFINITE);
	}

	SOCKET retVal = INVALID_SOCKET;
	
	EnterCriticalSection(&csSockets);

	for (USHORT i = 0; i < m_Sockets.size(); i++)
	{
		if (m_Sockets[i] == INVALID_SOCKET) {
			m_TakenSockets++;
			m_Sockets[i] = socket;
			retVal = m_Sockets[i];
			break;
		}
	}

	LeaveCriticalSection(&csSockets);

	return retVal;
}

void AcceptedSocketPool::ReturnSocketToPool(SOCKET socket)
{
	EnterCriticalSection(&csSockets);

	for (USHORT i = 0; i < m_Sockets.size(); i++)
	{
		if (m_Sockets[i] == socket) {
			shutdown(m_Sockets[i], SD_BOTH);
			closesocket(m_Sockets[i]);
			m_Sockets[i] = INVALID_SOCKET;
			m_TakenSockets--;
			WakeConditionVariable(&cvSockets);
			break;
		}
	}

	LeaveCriticalSection(&csSockets);
}