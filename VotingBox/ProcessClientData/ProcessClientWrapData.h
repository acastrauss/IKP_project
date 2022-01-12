#pragma once

#include <winsock2.h>
#include "AcceptedSocketPool.h"
#include "ThreadPool/ThreadPool.h"

/// <summary>
/// Used for thread that wrap client processing thread i.e.
/// Handling thread return to pool, closing used socket
/// </summary>
struct ProcessClientWrapData {
    SOCKET AcceptedSocket;
    DWORD ThreadId;
    Common::ThreadPool* ThreadPool;
    AcceptedSocketPool* SocketPool;

    ProcessClientWrapData() = delete;

    ProcessClientWrapData(
        SOCKET acceptedSocket,
        DWORD threadId,
        Common::ThreadPool* const threadPool,
        AcceptedSocketPool* const socketPool
    );

    ProcessClientWrapData(
        const ProcessClientWrapData& ref
    );

    ProcessClientWrapData& operator=(
        const ProcessClientWrapData& ref
    ) = delete;
};

