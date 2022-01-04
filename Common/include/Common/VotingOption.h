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
		
		explicit VotingOption(
			std::string PartyLeader,
			std::string PartyName,
			size_t PartyNumber
		);

		explicit VotingOption(char* buffer);

		VotingOption(
			const VotingOption& ref
		);

		VotingOption();

		char* Serialize();
		size_t BufferSize();


		VotingOption& operator=(const VotingOption& rhs);
		bool operator==(const VotingOption& rhs)const;
		bool operator!=(const VotingOption& rhs)const;
	};
}