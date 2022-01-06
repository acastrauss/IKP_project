#pragma once
#include "Common/Vote.h"
#include "Common/VotingOption.h"
#include "Common/Serializable.h"
#include <deque>
#include <vector>

namespace Common {
	/// <summary>
	/// VotingBox will send this to VotingCounters
	/// </summary>
	struct VotesToCount : Serializable
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
		
		size_t BufferSize() const;


		VotesToCount& operator=(
			const VotesToCount& rhs
		);
	};
}

