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

template<>
Common::VotingList Deserialize(char* buffer)
{
	ASSERT(buffer);

	size_t offset = 0;

	size_t numberOfOptions = *((size_t*)(buffer + offset));

	offset += sizeof(numberOfOptions);

	Common::VotingList votingList;

	for (size_t i = 0; i < numberOfOptions; i++)
	{
		votingList.AddOption(
			SubdataDeserialize<Common::VotingOption>(buffer, offset)
		);
	}

	return votingList;
}

template<>
Common::VotesToCount Deserialize(char* buffer)
{
	ASSERT(buffer);
	size_t offset = 0;

	size_t votesCount = *((size_t*)(buffer + offset));
	offset += sizeof(votesCount);

	size_t optionsCount = *((size_t*)(buffer + offset));
	offset += sizeof(optionsCount);

	std::deque<Common::Vote> votes = {};

	for (size_t i = 0; i < votesCount; i++)
	{
		votes.push_back(
			SubdataDeserialize<Common::Vote>(buffer, offset)
		);
	}

	std::vector<Common::VotingOption> options = {};

	for (size_t i = 0; i < optionsCount; i++)
	{
		options.push_back(
			SubdataDeserialize<Common::VotingOption>(buffer, offset)
		);
	}

	return Common::VotesToCount(
		votes, options
	);
}

template<>
Common::CountedVotes Deserialize(char* buffer)
{
	ASSERT(buffer);
	size_t offset = 0;

	size_t votesCount = *((size_t*)(buffer + offset));
	offset += sizeof(votesCount);

	std::map<size_t, size_t> votes = {};

	for (size_t i = 0; i < votesCount; i++)
	{
		size_t partyNumber = *((size_t*)(buffer + offset));
		offset += sizeof(partyNumber);
		size_t numberOfVotes = *((size_t*)(buffer + offset));
		offset += sizeof(numberOfVotes);

		votes.insert({
			partyNumber, numberOfVotes
		});
	}

	return Common::CountedVotes(votes);
}

template<>
Common::VotesContainer Deserialize(char* buffer)
{
	ASSERT(buffer);
	size_t offset = 0;

	std::deque<Common::Vote> votes = {};

	size_t votesCount = *((size_t*)(buffer + offset));
	offset += sizeof(votesCount);

	for (size_t i = 0; i < votesCount; i++)
	{
		votes.push_back(
			SubdataDeserialize<Common::Vote>(buffer, offset)
		);
	}

	return Common::VotesContainer(votes);
}

template<>
Common::FinalResult Deserialize(char* buffer)
{
	ASSERT(buffer);
	size_t offset = 0;

	size_t resultCount = *((size_t*)(buffer + offset));
	offset += sizeof(resultCount);

	std::vector<std::pair<Common::VotingOption, size_t>> results = {};

	for (size_t i = 0; i < resultCount; i++)
	{
		Common::VotingOption option = SubdataDeserialize<Common::VotingOption>(buffer, offset);
		size_t votesNumber = *((size_t*)(buffer + offset));
		offset += sizeof(votesNumber);

		results.push_back({
			option, votesNumber
		});
	}

	return Common::FinalResult(results);
}

#pragma region SubdataDeserialization

template<typename T>
T SubdataDeserialize(char* buffer, size_t& offset)
{
	static_assert(false);
}

template<>
Common::Vote SubdataDeserialize(char* buffer, size_t& offset)
{
	size_t voteBufferSize = *((size_t*)(buffer + offset));
	offset += sizeof(voteBufferSize);

	char* voteBuffer = new char[voteBufferSize];
	ASSERT(voteBuffer);

	memcpy(
		voteBuffer,
		buffer + offset,
		voteBufferSize
	);

	Common::Vote vote = Deserialize<Common::Vote>(voteBuffer);

	delete[] voteBuffer;

	offset += voteBufferSize;

	return vote;
}

template<>
Common::VotingOption SubdataDeserialize(char* buffer, size_t& offset)
{
	size_t optionBufferSize = *((size_t*)(buffer + offset));
	offset += sizeof(optionBufferSize);

	char* optionBuffer = new char[optionBufferSize];
	ASSERT(optionBuffer);

	memcpy(
		optionBuffer,
		buffer + offset,
		optionBufferSize
	);

	Common::VotingOption option = Deserialize<Common::VotingOption>(optionBuffer);

	delete[] optionBuffer;

	offset += optionBufferSize;

	return option;
}

#pragma endregion
