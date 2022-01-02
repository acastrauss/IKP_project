#pragma once

#include <map>
#include <Windows.h>
#include <deque>
#include <Common/Vote.h>
#include <vector>
#include <Common/VotingOption.h>

namespace Common {
	/// <summary>
	/// VotingCoutner will return this to VotingBox
	/// </summary>
	class CountedVotes
	{
	private:
		/// <summary>
		/// Key: Party Number
		/// Value: Number of votes
		/// </summary>
		std::map<size_t, size_t> Votes;

		CRITICAL_SECTION cs;

	public:
		CountedVotes();
		CountedVotes(
			std::map<size_t, size_t> votes
		);

		CountedVotes(
			const CountedVotes& rhs
		) = delete;

		~CountedVotes();

		/// <summary>
		/// When counting votes, after increase, delete vote from container!
		/// So that AssignZeroValues doesn't neet Vote container
		/// </summary>
		/// <param name="option"></param>
		void IncreaseVoteNumber(size_t option);
		
		/// <summary>
		/// Call after counting votes, to assign 0 votes to non voted parties
		/// </summary>
		/// <param name="options">Get from voting box</param>
		void AsssignZeroVotes(
			const std::vector<Common::VotingOption>& options
		);

		std::map<size_t, size_t> GetCountedVotes();

		CountedVotes& operator=(
			const CountedVotes& rhs
		) = delete;
	};
}


