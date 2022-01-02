#pragma once

#include <Common/Vote.h>
#include <deque>
#include <vector>
#include <Windows.h>
#include <mutex>

namespace Common {

	/// <summary>
	/// Class for representing voting results, stored at VotingBox
	/// </summary>
	class VotesContainer
	{
	private:
		std::deque<Common::Vote> m_Votes;

		CRITICAL_SECTION cs;
	public:
		VotesContainer();

		VotesContainer(
			const VotesContainer& rhs
		) = delete;

		~VotesContainer();

		VotesContainer& operator=(const VotesContainer& rhs) = delete;
	
		std::deque<Common::Vote> GetVotes();
		
		/// <summary>
		/// Returns roughly equally separated votes
		/// </summary>
		/// <param name="parts">Number of parts to separate to</param>
		/// <returns></returns>
		std::vector<
			std::deque<Common::Vote>
		> GetEquallySeparatedVotes(size_t parts);

		void AddVote(const Common::Vote& vote);
	};
}

