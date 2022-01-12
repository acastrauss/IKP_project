#pragma once
#include <winsock2.h>
#include <Common/VotingList.h>
#include <Common/VotesContainer.h>

/// <summary>
/// Used for thread that processes client 
/// i.e. sends options and receives vote
/// </summary>
struct ProcessClientData {
    SOCKET AcceptedSocket;
    const Common::VotingList& VotingList;
    Common::VotesContainer* const VotesContainerP;

    ProcessClientData() = delete;
    explicit ProcessClientData(
        SOCKET acceptedSocket,
        const Common::VotingList& votingList,
        Common::VotesContainer * const votesContainerP
    );

    ProcessClientData(
        const ProcessClientData& ref
    );

    ProcessClientData& operator=(
        const ProcessClientData& ref
    ) = delete;
};