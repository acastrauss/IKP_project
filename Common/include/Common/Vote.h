#pragma once
#include <ctime>
#include <chrono>
#include "Common/Serializable.h"

namespace Common {
	/// <summary>
	/// Client's vote
	/// </summary>
	struct Vote : Serializable
	{
		long long VoterId;
		size_t PartyNumber;
		/// <summary>
		/// Note: Time should be set on server side
		/// </summary>
		std::time_t VoteTime;

		Vote();

		explicit Vote(
			long long voterId,
			size_t partyNumber,
			std::time_t voteTime = time(0)
		);

		Vote(
			const Vote& rhs
		);

		size_t BufferSize() const;

		Vote& operator=(const Vote& rhs);
	};
}
