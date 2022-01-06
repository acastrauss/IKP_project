#include "pch.h"
#include <iostream>
#include "Common/VotingOption.h"
#include "Common/CommonMethods.h"

namespace Common {
	VotingOption::VotingOption()
	{
		this->PartyLeader = "";
		this->PartyName = "";
		this->PartyNumber = 0;
	}

	VotingOption::VotingOption
		(std::string PartyLeader, std::string PartyName, size_t PartyNumber)
	{
		this->PartyLeader = PartyLeader;
		this->PartyName = PartyName;
		this->PartyNumber = PartyNumber;
	}

	VotingOption::VotingOption(const VotingOption& ref)
	{
		this->PartyLeader = ref.PartyLeader;
		this->PartyName = ref.PartyName;
		this->PartyNumber = ref.PartyNumber;
	}

	size_t VotingOption::BufferSize() const{
		return
			PartyLeader.size() +
			PartyName.size() +
			sizeof(PartyNumber) +
			sizeof(size_t) * 2;
	}

	VotingOption& VotingOption::operator=(const VotingOption& rhs)
	{
		this->PartyLeader = rhs.PartyLeader;
		this->PartyName = rhs.PartyName;
		this->PartyNumber = rhs.PartyNumber;
		return *this;
	}

	bool VotingOption::operator==(const VotingOption& rhs) const
	{
		return
			this->PartyLeader == rhs.PartyLeader &&
			this->PartyName == rhs.PartyName &&
			this->PartyNumber == rhs.PartyNumber;
	}

	bool VotingOption::operator!=(const VotingOption& rhs) const
	{
		return !this->operator==(rhs);
	}
}