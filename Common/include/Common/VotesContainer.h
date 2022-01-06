#pragma once

#include "Common/Vote.h"
#include "Common/Serializable.h"
#include <deque>
#include <vector>
#include <Windows.h>
#include <mutex>

namespace Common {

	/// <summary>
	/// Class for representing voting results, stored at VotingBox
	/// </summary>
	class VotesContainer : Serializable
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

		size_t BufferSize() const;

		VotesContainer& operator=(const VotesContainer& rhs) = delete;
	
		std::deque<Common::Vote> GetVotes() const;
		
		/// <summary>
		/// Returns roughly equally separated votes
		/// </summary>
		/// <param name="parts">Number of parts to separate to</param>
		/// <returns></returns>
		std::vector<
			std::deque<Common::Vote>
		> GetEquallySeparatedVotes(size_t parts) const;

		void AddVote(const Common::Vote& vote);
	};
}

