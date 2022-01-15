#pragma once

#include <WinSock2.h>
#include <Common/VotesContainer.h>
#include <Common/VotingList.h>

struct ClientThreadParam {
    SOCKET AcceptedSocket;
    Common::VotesContainer* VotesContainer;
    Common::VotingList VotingOptions;

    ClientThreadParam() = delete;

    ClientThreadParam(
        SOCKET acceptedSocket,
        Common::VotesContainer* votesContainer,
        Common::VotingList votingOptions
    );

    ClientThreadParam(
        const ClientThreadParam& ref
    );

    ClientThreadParam& operator=(
        const ClientThreadParam& rhs
        );
};