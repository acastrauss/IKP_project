#include "pch.h"
#include "Common/FinalResult.h"
#include <Common/CommonMethods.h>


namespace Common {
	FinalResult::FinalResult()
	{
		Results = {};
	}

	FinalResult::FinalResult(
		std::map<size_t, size_t> countedVotes,
		std::vector<Common::VotingOption> allOptions)
	{
		for (auto it = countedVotes.begin(); it != countedVotes.end(); it++)
		{
			auto itOpt = std::find_if(
				allOptions.begin(),
				allOptions.end(),
				[=](const VotingOption& ref) {
					return ref.PartyNumber == it->first;
				});

			ASSERT(itOpt != allOptions.end());

			Results.push_back({
				*itOpt, it->second
			});
		}
	}

	FinalResult::FinalResult(const FinalResult& rhs)
	{
		Results = rhs.Results;
	}

	FinalResult& FinalResult::operator=(const FinalResult& rhs)
	{
		Results = rhs.Results;
		return *this;
	}
}