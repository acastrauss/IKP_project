#pragma once

#include "Common/VotingOption.h"
#include <vector>
#include <map>

namespace Common {
	/// <summary>
	/// VotingBox will send this to InformationServer
	/// Represents final results of elections
	/// </summary>
	struct FinalResult
	{
		std::vector<std::pair<Common::VotingOption, size_t>> Results;

		FinalResult();
		FinalResult(
			std::map<size_t, size_t> countedVotes,
			std::vector<Common::VotingOption> allOptions
		);

		FinalResult(
			const FinalResult& rhs
		);

		FinalResult& operator=(
			const FinalResult & rhs
		);
	};
}
