#include "pch.h"
#include "Common/CountedVotes.h"

namespace Common {
	CountedVotes::CountedVotes()
	{
		Votes = std::map<size_t, size_t>();
		InitializeCriticalSection(&cs);
	}

	CountedVotes::CountedVotes(std::map<size_t, size_t> votes)
	{
		Votes = votes;
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
			Votes.find(option) != Votes.end()
			) {
			Votes[option]++;
		}
		else {
			Votes.insert({
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
			if (Votes.find(it->PartyNumber) == Votes.end()) {
				Votes.insert({
					it->PartyNumber,
					0
				});
			}
		}

		LeaveCriticalSection(&cs);	
	}

	std::map<size_t, size_t> Common::CountedVotes::GetCountedVotes() const
	{
		return Votes;
	}
}