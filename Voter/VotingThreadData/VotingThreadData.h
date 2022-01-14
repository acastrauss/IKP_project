#pragma once

#include "Common/VotingList.h"
#include "Common/VotesContainer.h"

struct VotingThreadData {
    UINT NumberOfVotesPerThread;
    const Common::VotingList& VotingList;
    Common::VotesContainer* VotesContainer;

    VotingThreadData() = delete;

    VotingThreadData(
        UINT numberOfVotesPerThread,
        const Common::VotingList& votingList,
        Common::VotesContainer* votesContainerP
    );

    VotingThreadData(
        const VotingThreadData& ref
    );

    VotingThreadData& operator=(
        const VotingThreadData& rhs
        ) = delete;
};
