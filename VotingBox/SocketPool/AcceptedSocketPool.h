#pragma once
#include <winsock2.h>
#include <vector>

class AcceptedSocketPool
{
private:
	std::vector<SOCKET> m_Sockets;
	USHORT m_TakenSockets;
	CONDITION_VARIABLE cvSockets;
	CRITICAL_SECTION csSockets;

public:

	explicit AcceptedSocketPool(USHORT socketNum);

	~AcceptedSocketPool();

	AcceptedSocketPool() = delete;
	AcceptedSocketPool(
		const AcceptedSocketPool& ref
	) = delete;

	AcceptedSocketPool& operator=(
		const AcceptedSocketPool& ref
		) = delete;

	/// <summary>
	/// Assigns new socket to first empty space
	/// Returns the same socket so it can be used in program
	/// </summary>
	/// <param name="newSocket"></param>
	SOCKET AssignNewSocket(SOCKET socket);
	void ReturnSocketToPool(SOCKET socket);
};

