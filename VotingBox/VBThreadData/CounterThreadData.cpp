#include "CounterThreadData.h"

CounterThreadData::CounterThreadData(
	SOCKET socket,
	Common::VotesToCount votesToCount,
	AssembleVotes* assembleVotesP
)
: 
	ConnectSocket(socket),
	VotesToCount(votesToCount),
	AssembledVotesP(assembleVotesP)
{}

CounterThreadData::CounterThreadData(const CounterThreadData & ref)
: 
	ConnectSocket(ref.ConnectSocket),
	VotesToCount(ref.VotesToCount),
	AssembledVotesP(ref.AssembledVotesP)
{}
