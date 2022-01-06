#include "pch.h"
#include "Serialization/Deserialization.h"
#include "Common/CommonMethods.h"
#pragma warning(disable:6385)

template<typename T>
inline T Deserialize(char* buffer)
{
	static_assert(false);
}

template<>
Common::Vote Deserialize(char* buffer) 
{
	ASSERT(buffer);

	Common::Vote vote;

	size_t offset = 0;

	vote.VoterId = *((long long*)(buffer + offset));

	offset += sizeof(long long);

	vote.PartyNumber = *((size_t*)(buffer + offset));

	offset += sizeof(size_t);

	vote.VoteTime = *((time_t*)(buffer + offset));

	return vote;
}

template<>
Common::VotingOption Deserialize(char* buffer)
{
	ASSERT(buffer);

	Common::VotingOption votingOption;

	size_t offset = 0;

	size_t partyLeaderSize = *((size_t*)(buffer + offset));
	offset += sizeof(partyLeaderSize);

	size_t partyNameSize = *((size_t*)(buffer + offset));
	offset += sizeof(partyNameSize);

	// party leader deserialization
	char* partyLeaderCstr = new char[partyLeaderSize];

	memcpy(
		partyLeaderCstr,
		buffer + offset,
		partyLeaderSize
	);

	char temp = partyLeaderCstr[partyLeaderSize];

	partyLeaderCstr[partyLeaderSize] = '\0';

	votingOption.PartyLeader = std::string(partyLeaderCstr);

	partyLeaderCstr[partyLeaderSize] = temp;

	offset += partyLeaderSize;

	delete[] partyLeaderCstr;

	// party name deserialization
	char* partyNameCstr = new char[partyNameSize];

	memcpy(
		partyNameCstr,
		buffer + offset,
		partyNameSize
	);

	temp = partyNameCstr[partyNameSize];

	partyNameCstr[partyNameSize] = '\0';

	votingOption.PartyName = std::string(partyNameCstr);

	partyNameCstr[partyNameSize] = temp;

	delete[] partyNameCstr;

	offset += partyNameSize;

	votingOption.PartyNumber = *((size_t*)(buffer + offset));

	return votingOption;
}
