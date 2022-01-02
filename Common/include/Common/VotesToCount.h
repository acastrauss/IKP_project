#pragma once
#include "Common/Vote.h"
#include "Common/VotingOption.h"
#include <deque>
#include <vector>

namespace Common {
	/// <summary>
	/// VotingBox will send this to VotingCounters
	/// </summary>
	struct VotesToCount
	{
		std::deque<Common::Vote> Votes;
		std::vector<Common::VotingOption> Options;

		VotesToCount();
		explicit VotesToCount(
			std::deque<Common::Vote> votes,
			std::vector<Common::VotingOption> options
		);
		VotesToCount(
			const VotesToCount& rhs
		);

		VotesToCount& operator=(
			const VotesToCount& rhs
		);
	};
}

