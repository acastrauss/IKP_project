#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Common/VotesToCount.h>
#include "AssembleCountedVotes/AssembleVotes.h"

/// <summary>
/// Data passed to thread that calls votes counter
/// </summary>
struct CounterThreadData {
    SOCKET ConnectSocket;
    Common::VotesToCount VotesToCount;
    AssembleVotes* AssembledVotesP;

    CounterThreadData() = delete;
    explicit CounterThreadData(
        SOCKET socket,
        Common::VotesToCount votesToCount,
        AssembleVotes* assembleVotesP
    );
    CounterThreadData(
        const CounterThreadData& ref
    );
};