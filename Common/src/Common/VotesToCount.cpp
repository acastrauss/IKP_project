#include "pch.h"
#include "Common/VotesToCount.h"
#include <algorithm>


namespace Common {
	VotesToCount::VotesToCount()
	{
		Votes = std::deque<Common::Vote>();
	}

	VotesToCount::VotesToCount(
		const std::deque<Common::Vote>& votes,
		const std::vector<Common::VotingOption>& options)
	{
		Votes = votes;
		Options = options;
	}

	VotesToCount::VotesToCount(const VotesToCount& rhs)
	{
		Votes = rhs.Votes;
		Options = rhs.Options;
	}

	size_t Common::VotesToCount::BufferSize() const
	{
		size_t bufferSize = 0;

		// metadata
		bufferSize += sizeof(Votes.size());
		bufferSize += sizeof(Options.size());

		std::for_each(
			Votes.begin(),
			Votes.end(),
			[&](const Common::Vote& vote) {
				bufferSize += sizeof(vote.BufferSize());
				bufferSize += vote.BufferSize();
			}
		);

		std::for_each(
			Options.begin(),
			Options.end(),
			[&](const Common::VotingOption& option) {
				bufferSize += sizeof(option.BufferSize());
				bufferSize += option.BufferSize();
			}
		);

		return bufferSize;
	}

	VotesToCount& VotesToCount::operator=(const VotesToCount& rhs)
	{
		Votes = rhs.Votes;
		Options = rhs.Options;
		return *this;
	}
}	


