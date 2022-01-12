#include "pch.h"
#include "Serialization/Serialization.h"
#include <Common/CommonMethods.h>
#include <algorithm>

#pragma warning(disable:6386)

template<typename T>
inline char* Serialize(const T& obj)
{
	static_assert(false);
}

template<>
char* Serialize(const Common::Vote& obj)
{
	size_t bufferSize = obj.BufferSize();

	char* buffer = new char[bufferSize];

	ASSERT(buffer);

	size_t offset = 0;

	memcpy(
		buffer + offset,
		&obj.VoterId,
		sizeof(obj.VoterId)
	);
	
	offset += sizeof(obj.VoterId);

	memcpy(
		buffer + offset,
		&obj.PartyNumber,
		sizeof(obj.PartyNumber)
	);

	offset += sizeof(obj.PartyNumber);

	memcpy(
		buffer + offset,
		&obj.VoteTime,
		sizeof(obj.VoteTime)
	);


	return buffer;
}

template<>
char* Serialize(const Common::VotingOption& obj)
{
	size_t bufferSize = obj.BufferSize();
	char* buffer = new char[bufferSize];

	ASSERT(buffer);

	size_t offset = 0;
	
	size_t partyLeaderSize = obj.PartyLeader.size();

	memcpy(
		buffer + offset,
		&partyLeaderSize,
		sizeof(partyLeaderSize)
	);

	offset += sizeof(partyLeaderSize);

	size_t partyNameSize = obj.PartyName.size();

	memcpy(
		buffer + offset,
		&partyNameSize,
		sizeof(partyNameSize)
	);

	offset += sizeof(partyNameSize);

	memcpy(
		buffer + offset,
		obj.PartyLeader.c_str(),
		obj.PartyLeader.size()
	);

	offset += obj.PartyLeader.size();
	
	memcpy(
		buffer + offset,
		obj.PartyName.c_str(),
		obj.PartyName.size()
	);

	offset += obj.PartyName.size();

	memcpy(
		buffer + offset,
		&obj.PartyNumber,
		sizeof(obj.PartyNumber)
	);
	
	return buffer;
}

template<>
char* Serialize(const Common::VotingList& obj)
{
	size_t bufferSize = obj.BufferSize();

	char* buffer = new char[bufferSize];
	
	ASSERT(buffer);
	
	std::vector<Common::VotingOption> options = obj.GetOptions();

	size_t optionsCount = options.size();
	
	size_t offset = 0;
	
	memcpy(
		buffer + offset,
		&optionsCount,
		sizeof(optionsCount)
	);

	size_t optN = *((size_t*)(buffer + offset));

	offset += sizeof(optionsCount);

	std::for_each(
		options.begin(),
		options.end(),
		[&](const Common::VotingOption& opt) {
			AddSubdataToBuffer(buffer, opt, offset);
		}
	);

	return buffer;
}

template<>
char* Serialize(const Common::VotesToCount& obj)
{
	size_t bufferSize = obj.BufferSize();
	char* buffer = new char[bufferSize];
	size_t offset = 0;

	size_t votesSize = obj.Votes.size();

	memcpy(
		buffer + offset,
		&votesSize,
		sizeof(votesSize)
	);

	offset += sizeof(votesSize);

	size_t optionsSize = obj.Options.size();

	memcpy(
		buffer + offset,
		&optionsSize,
		sizeof(optionsSize)
	);

	offset += sizeof(optionsSize);

	std::for_each(
		obj.Votes.begin(),
		obj.Votes.end(),
		[&](const Common::Vote& vote) {
			AddSubdataToBuffer(buffer, vote, offset);
		}
	);

	std::for_each(
		obj.Options.begin(),
		obj.Options.end(),
		[&](const Common::VotingOption& option) {
			AddSubdataToBuffer(buffer, option, offset);
		}
	);

	return buffer;
}

template<>
char* Serialize(const Common::CountedVotes& obj)
{
	size_t bufferSize = obj.BufferSize();
	char* buffer = new char[bufferSize];
	size_t offset = 0;

	std::map<size_t, size_t> votes = obj.GetCountedVotes();
	size_t votesSize = votes.size();

	memcpy(
		buffer,
		&votesSize,
		sizeof(votesSize)
	);

	offset += sizeof(votesSize);

	std::for_each(
		votes.begin(),
		votes.end(),
		[&](std::pair<size_t, size_t> votesCount) {

			memcpy(
				buffer+offset,
				&votesCount.first,
				sizeof(votesCount.first)
			);

			offset += sizeof(votesCount.first);

			memcpy(
				buffer+offset,
				&votesCount.second,
				sizeof(votesCount.second)
			);

			offset += sizeof(votesCount.second);
		}
	);

	return buffer;
}

template<>
char* Serialize(const Common::VotesContainer& obj)
{
	size_t bufferSize = obj.BufferSize();
	std::deque<Common::Vote> votes = obj.GetVotes();

	char* buffer = new char[bufferSize];
	size_t offset = 0;


	size_t votesSize = votes.size();

	memcpy(
		buffer + offset,
		&votesSize,
		sizeof(votesSize)
	);

	offset += sizeof(votesSize);

	std::for_each(
		votes.begin(),
		votes.end(),
		[&](const Common::Vote vote) {
			AddSubdataToBuffer(buffer, vote, offset);
		}
	);

	return buffer;
}

template<>
char* Serialize(const Common::FinalResult& obj)
{
	size_t bufferSize = obj.BufferSize();
	char* buffer = new char[bufferSize];
	size_t offset = 0;

	size_t resultsSize = obj.Results.size();

	memcpy(
		buffer + offset,
		&resultsSize,
		sizeof(resultsSize)
	);

	offset += sizeof(resultsSize);

	std::for_each(
		obj.Results.begin(),
		obj.Results.end(),
		[&](const std::pair<Common::VotingOption, size_t>& result) {
			AddSubdataToBuffer(buffer, result.first, offset);

			memcpy(
				buffer+offset,
				&result.second,
				sizeof(result.second)
			);

			offset += sizeof(result.second);
		}
	);

	return buffer;
}


#pragma region SubdataSerialization

template<typename T>
inline void AddSubdataToBuffer(char* buffer, const T& obj, size_t& offset)
{
	static_assert(false);
}

template<>
void AddSubdataToBuffer(char* buffer, const Common::Vote& obj, size_t& offset)
{
	size_t voteBufferSize = obj.BufferSize();
	

	memcpy(
		buffer + offset,
		&voteBufferSize,
		sizeof(voteBufferSize)
	);

	offset += sizeof(voteBufferSize);
	
	char* voteBuffer = Serialize(obj);
	
	memcpy(
		buffer + offset,
		voteBuffer,
		voteBufferSize
	);

	offset += voteBufferSize;

	delete[] voteBuffer;
}

template<>
void AddSubdataToBuffer(char* buffer, const Common::VotingOption& obj, size_t& offset)
{
	size_t optionBufferSize = obj.BufferSize();

	memcpy(
		buffer + offset,
		&optionBufferSize,
		sizeof(optionBufferSize)
	);

	offset += sizeof(optionBufferSize);
	
	char* optionBuffer = Serialize(obj);

	memcpy(
		buffer + offset,
		optionBuffer,
		optionBufferSize
	);

	offset += optionBufferSize;
	delete[] optionBuffer;
}

#pragma endregion

