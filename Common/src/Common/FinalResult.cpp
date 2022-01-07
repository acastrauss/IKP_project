#include "pch.h"
#include "Common/FinalResult.h"
#include <Common/CommonMethods.h>
#include <algorithm>


namespace Common {
	FinalResult::FinalResult()
	{
		Results = {};
	}

	FinalResult::FinalResult(
		const std::map<size_t, size_t>& countedVotes,
		const std::vector<Common::VotingOption>& allOptions)
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

	FinalResult::FinalResult
		(const std::vector<std::pair<Common::VotingOption, size_t>>& results)
	{
		Results = results;
	}

	FinalResult::FinalResult(const FinalResult& rhs)
	{
		Results = rhs.Results;
	}

	size_t Common::FinalResult::BufferSize() const
	{
		size_t bufferSize = 0;

		bufferSize += sizeof(Results.size());

		std::for_each(
			Results.begin(),
			Results.end(),
			[&](const std::pair<Common::VotingOption, size_t> result) {
				bufferSize += sizeof(result.first.BufferSize());
				bufferSize += result.first.BufferSize();
				bufferSize += sizeof(result.second);
			}
		);

		return bufferSize;
	}

	FinalResult& FinalResult::operator=(const FinalResult& rhs)
	{
		Results = rhs.Results;
		return *this;
	}
}