#pragma once

#include "Common/VotingOption.h"
#include "Common/Serializable.h"
#include <vector>
#include <map>

namespace Common {
	/// <summary>
	/// VotingBox will send this to InformationServer
	/// Represents final results of elections
	/// </summary>
	struct FinalResult : Serializable
	{
		std::vector<std::pair<Common::VotingOption, size_t>> Results;

		FinalResult();

		explicit FinalResult(
			const std::vector<std::pair<Common::VotingOption, size_t>>& results
		);

		explicit FinalResult(
			const std::map<size_t, size_t>& countedVotes,
			const std::vector<Common::VotingOption>& allOptions
		);

		FinalResult(
			const FinalResult& rhs
		);

		size_t BufferSize() const;

		FinalResult& operator=(
			const FinalResult & rhs
		);
	};
}
