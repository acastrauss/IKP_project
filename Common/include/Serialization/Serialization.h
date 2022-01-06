#pragma once

#include "Common/Vote.h"
#include "Common/CountedVotes.h"
#include "Common/FinalResult.h"
#include "Common/VotesContainer.h"
#include "Common/VotesToCount.h"
#include "Common/VotingList.h"
#include "Common/VotingOption.h"

/// <summary>
/// Template function for serialization
/// Note: use template specializations!
/// Note: return buffer must be freed/deleted!
/// </summary>
/// <typeparam name="T">Common type for serialziation</typeparam>
/// <param name="obj"></param>
/// <returns>
/// Char buffer with structure:
/// fieldName(fieldSize)
/// mMetaDataName(metaDataSize)
/// [array of data]
/// {key, value} pair
/// </returns>
template<typename T>
char* Serialize(const T& obj);

/// <summary>
/// 
/// </summary>
/// <param name="obj"></param>
/// <returns>
/// Char buffer with structure:
/// VoterId(sizeof(VoterId)),
/// PartyNumber(sizeof(PartyNumber)),
/// VoteTime(sizeof(VoteTime))
/// </returns>
template<>
char* Serialize(const Common::Vote& obj);

/// <summary>
/// 
/// </summary>
/// <param name="obj"></param>
/// <returns>
/// mPartyLeaderSize(sizeof(size_t)),
/// mPartyNameSize(sizeof(size_t)),
/// PartyLeader(PartyLeader.size()),
/// PartyName(PartyName.size()),
/// PartyNumber(sizeof(PartyNumber))
/// </returns>
template<>
char* Serialize(const Common::VotingOption& obj);

/// <summary>
/// 
/// </summary>
/// <param name="obj"></param>
/// <returns>
/// mNumberOfOptions(m_Options.length()),
/// [
/// VotingOptionBufferSize,
/// VotingOptionSerialized
/// ]
/// </returns>
template<>
char* Serialize(const Common::VotingList& obj);

/// <summary>
/// 
/// </summary>
/// <param name="obj"></param>
/// <returns>
/// mNumberOfVotes(Votes.length()),
/// mNumberOfVotingOptions(Options.length()),
/// [
/// VoteBufferSize,
/// VoteSerialized
/// ]
/// [
/// VotingOptionBufferSize,
/// VotingOptionSerialized
/// ]
/// </returns>
template<>
char* Serialize(const Common::VotesToCount& obj);

/// <summary>
/// 
/// </summary>
/// <param name="obj"></param>
/// <returns>
/// mNumberOfVotes(Votes.length()),
/// [{
///		PartyNumber(sizeof(PartyNumber)),
///		NumberOfVotes(sizeof(NumberOfVotes))
/// }]
/// </returns>
template<>
char* Serialize(const Common::CountedVotes& obj);

/// <summary>
/// 
/// </summary>
/// <param name="obj"></param>
/// <returns>
/// mNumberOfVotes
/// [
/// VoteBufferSize,
/// VoteSerialized
/// ]
/// </returns>
template<>
char* Serialize(const Common::VotesContainer& obj);

/// <summary>
/// 
/// </summary>
/// <param name="obj"></param>
/// <returns>
/// mNumberOfResults
/// [
/// VotingOptionBufferSize,
/// VotingOptionSerialized,
/// NumberOfVotes
/// ]
/// </returns>
template<>
char* Serialize(const Common::FinalResult& obj);


#pragma region SubdataSerialization

/// <summary>
/// Template method for serializing subdata,
/// inside of serialization of containers
/// e.g. When serializing VotesList, to serialize Vote
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="buffer"></param>
/// <param name="obj"></param>
/// <param name="offset"></param>
template<typename T>
void AddSubdataToBuffer(
	char* buffer, const T& obj, size_t& offset
);

template<>
void AddSubdataToBuffer(
	char* buffer, const Common::Vote& obj, size_t& offset
);

template<>
void AddSubdataToBuffer(
	char* buffer, const Common::VotingOption& obj, size_t& offset
);

#pragma endregion

