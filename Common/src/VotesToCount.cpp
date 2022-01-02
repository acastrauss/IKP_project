#include "pch.h"
#include "Common/VotesToCount.h"


namespace Common {
	VotesToCount::VotesToCount()
	{
		Votes = std::deque<Common::Vote>();
	}

	VotesToCount::VotesToCount(
		std::deque<Common::Vote> votes,
		std::vector<Common::VotingOption> options)
	{
		Votes = votes;
		Options = options;
	}

	VotesToCount::VotesToCount(const VotesToCount& rhs)
	{
		Votes = rhs.Votes;
		Options = rhs.Options;
	}

	VotesToCount& VotesToCount::operator=(const VotesToCount& rhs)
	{
		Votes = rhs.Votes;
		Options = rhs.Options;
		return *this;
	}
}	


