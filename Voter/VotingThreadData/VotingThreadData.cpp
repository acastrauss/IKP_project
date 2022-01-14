#include "VotingThreadData.h"

VotingThreadData::VotingThreadData
   (UINT numberOfVotesPerThread,
    const Common::VotingList& votingList,
    Common::VotesContainer* votesContainerP):
    NumberOfVotesPerThread(numberOfVotesPerThread),
    VotingList(votingList),
    VotesContainer(votesContainerP)
{}

VotingThreadData::VotingThreadData(const VotingThreadData & ref):
    NumberOfVotesPerThread(ref.NumberOfVotesPerThread),
    VotingList(ref.VotingList),
    VotesContainer(ref.VotesContainer)
{}
