#include "ProcessClientData.h"

ProcessClientData::ProcessClientData
	(SOCKET acceptedSocket, const Common::VotingList& votingList, Common::VotesContainer* const votesContainerP)
	:
	AcceptedSocket(acceptedSocket),
	VotingList(votingList),
	VotesContainerP(votesContainerP)
{}

ProcessClientData::ProcessClientData(const ProcessClientData& ref)
	:
	AcceptedSocket(ref.AcceptedSocket),
	VotingList(ref.VotingList),
	VotesContainerP(ref.VotesContainerP)
{}
