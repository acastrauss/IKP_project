#pragma once

#include <string>
#include "Serializable.h"

namespace Common {
	/// <summary>
	/// Struct representing model of one option to vote for in elections.
	/// </summary>
	struct VotingOption : Common::Serializable {
		std::string PartyLeader;
		std::string PartyName;
		size_t PartyNumber;
		
		VotingOption(
			std::string PartyLeader,
			std::string PartyName,
			size_t PartyNumber
		);

		VotingOption(
			const VotingOption& ref
		);

		VotingOption();

		size_t BufferSize() const;

		VotingOption& operator=(const VotingOption& rhs);
		bool operator==(const VotingOption& rhs)const;
		bool operator!=(const VotingOption& rhs)const;
	};
}