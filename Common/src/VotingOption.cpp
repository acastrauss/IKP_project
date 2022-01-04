#include "pch.h"
#include <iostream>
#include "Common/VotingOption.h"
#include "Common/CommonMethods.h"
#pragma warning(disable:6386)

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

	char* VotingOption::Serialize() 
	{
		/// <summary>
		/// First 2 elements in serialization will be used 
		/// as heading to set size of strings
		/// </summary>
		/// <returns></returns>

		size_t size = PartyLeader.size() +
			PartyName.size() +
			sizeof(PartyNumber) + 
			sizeof(size_t) * 2  // for string sizes 
			;

		char* buffer = new char[size];

		ASSERT(buffer);

		size_t plSize = PartyLeader.size();

		// size of first string
		memcpy(
			buffer,
			&plSize,
			sizeof(size_t)
		);
		
		size_t pnSize = PartyName.size();

		// size of second string
		memcpy(
			buffer + sizeof(size_t),
			&pnSize,
			sizeof(size_t)
		);

		memcpy(
			buffer + sizeof(size_t) * 2,
			PartyLeader.c_str(),
			PartyLeader.size()
		);
		
		memcpy(
			buffer + PartyLeader.size() + sizeof(size_t) * 2,
			PartyName.c_str(),
			PartyName.size()
		);
		
		memcpy(
			buffer + PartyLeader.size() + PartyName.size() + sizeof(size_t) * 2,
			&PartyNumber,
			sizeof(PartyNumber)
		);

		return buffer;
	}
	
	size_t VotingOption::BufferSize() {
		return
			PartyLeader.size() +
			PartyName.size() +
			sizeof(PartyNumber) +
			sizeof(size_t) * 2;
	}

	Common::VotingOption::VotingOption(char* buffer)
	{
		ASSERT(buffer);

		size_t partyLeaderSize = *((size_t*)buffer);
		size_t partyNameSize = *((size_t*)(buffer + sizeof(size_t)));

		char* plCp = new char[partyLeaderSize + 1];
		memcpy(
			plCp,
			buffer + sizeof(size_t) * 2,
			partyLeaderSize
		);

		plCp[partyLeaderSize] = '\0';

		PartyLeader = std::string(plCp);

		delete[] plCp;

		char* pNCp = new char[partyNameSize + 1];
		memcpy(
			pNCp,
			buffer + sizeof(size_t) * 2 + partyLeaderSize,
			partyNameSize
		);

		pNCp[partyNameSize] = '\0';

		PartyName = std::string(pNCp);

		delete[] pNCp;

		PartyNumber = *((size_t*)
			(
				buffer +
				sizeof(size_t) * 2 +
				partyLeaderSize +
				partyNameSize
			)
		);
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