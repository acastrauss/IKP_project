#include "pch.h"
#include "Common/Vote.h"

namespace Common {
	Vote::Vote()
	{
		VoterId = -1;
		PartyNumber = 0;
		VoteTime = time(0);
	}

	Vote::Vote(long long voterId, size_t partyNumber, std::time_t voteTime)
	{
		VoterId = voterId;
		PartyNumber = partyNumber;
		VoteTime = voteTime;
	}

	Vote::Vote(const Vote& rhs)
	{
		this->VoterId = rhs.VoterId;
		this->PartyNumber = rhs.PartyNumber;
		this->VoteTime = rhs.VoteTime;
	}

	size_t Vote::BufferSize() const
	{
		return
			sizeof(VoterId) +
			sizeof(PartyNumber) +
			sizeof(VoteTime);
	}

	Vote& Vote::operator=(const Vote& rhs)
	{
		this->VoterId = rhs.VoterId;
		this->PartyNumber = rhs.PartyNumber;
		this->VoteTime = rhs.VoteTime;
		return *this;
	}
}