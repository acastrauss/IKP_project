#include "ClientThreadParam.h"

ClientThreadParam::ClientThreadParam(
	SOCKET acceptedSocket,
	Common::VotesContainer* votesContainer,
	Common::VotingList votingOptions
):
	AcceptedSocket(acceptedSocket),
	VotesContainer(votesContainer),
	VotingOptions(votingOptions)
{}

ClientThreadParam::ClientThreadParam(const ClientThreadParam& ref):
	AcceptedSocket(ref.AcceptedSocket),
	VotesContainer(ref.VotesContainer),
	VotingOptions(ref.VotingOptions)
{}

ClientThreadParam& ClientThreadParam::operator=(const ClientThreadParam& rhs)
{
	AcceptedSocket = rhs.AcceptedSocket;
	VotesContainer = rhs.VotesContainer;
	VotingOptions = rhs.VotingOptions;
	return *this;
}