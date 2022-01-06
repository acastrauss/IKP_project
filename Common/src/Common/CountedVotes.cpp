#include "pch.h"
#include "Common/CountedVotes.h"

namespace Common {
	CountedVotes::CountedVotes()
	{
		m_Votes = std::map<size_t, size_t>();
		InitializeCriticalSection(&cs);
	}

	CountedVotes::CountedVotes(std::map<size_t, size_t> votes)
	{
		m_Votes = votes;
		InitializeCriticalSection(&cs);
	}

	Common::CountedVotes::~CountedVotes()
	{
		DeleteCriticalSection(&cs);
	}

	void CountedVotes::IncreaseVoteNumber(size_t option)
	{
		EnterCriticalSection(&cs);

		if (
			m_Votes.find(option) != m_Votes.end()
			) {
			m_Votes[option]++;
		}
		else {
			m_Votes.insert({
				option, 1
			});
		}

		LeaveCriticalSection(&cs);
	}

	void CountedVotes::AsssignZeroVotes(const std::vector<Common::VotingOption>& options)
	{
		EnterCriticalSection(&cs);

		for (auto it = options.begin(); it != options.end(); it++)
		{
			if (m_Votes.find(it->PartyNumber) == m_Votes.end()) {
				m_Votes.insert({
					it->PartyNumber,
					0
				});
			}
		}

		LeaveCriticalSection(&cs);	
	}

	size_t Common::CountedVotes::BufferSize() const
	{
		/// <summary>
		/// Because m_Votes consist of size_t key-value pairs
		/// </summary>
		/// <returns></returns>
		return m_Votes.size() * sizeof(size_t) * 2;
	}

	std::map<size_t, size_t> Common::CountedVotes::GetCountedVotes() const
	{
		return m_Votes;
	}
}