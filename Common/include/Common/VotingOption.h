#pragma once

#include <string>

namespace Common {
	/// <summary>
	/// Struct representing model of one option to vote for in elections.
	/// </summary>
	struct VotingOption {
		std::string PartyLeader;
		std::string PartyName;
		size_t PartyNumber;
		
		explicit VotingOption(
			std::string PartyLeader,
			std::string PartyName,
			size_t PartyNumber
		);

		VotingOption(
			const VotingOption& ref
		);

		VotingOption();

		VotingOption& operator=(const VotingOption& rhs);
		bool operator==(const VotingOption& rhs)const;
		bool operator!=(const VotingOption& rhs)const;
	};
}