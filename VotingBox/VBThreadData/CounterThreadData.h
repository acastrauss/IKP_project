#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Common/VotesToCount.h>

/// <summary>
/// Data passed to thread that calls votes counter
/// </summary>
struct CounterThreadData {
    SOCKET ConnectSocket;
    Common::VotesToCount VotesToCount;

    CounterThreadData() = delete;
    explicit CounterThreadData(
        SOCKET socket,
        Common::VotesToCount votesToCount
    );
    CounterThreadData(
        const CounterThreadData& ref
    );
};