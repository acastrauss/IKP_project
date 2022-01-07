#pragma once

#include "Common/Vote.h"
#include "Common/CountedVotes.h"
#include "Common/FinalResult.h"
#include "Common/VotesContainer.h"
#include "Common/VotesToCount.h"
#include "Common/VotingList.h"
#include "Common/VotingOption.h"


/// <summary>
/// Template method for deserializing system models.
/// Use template specizalizations!
/// For buffer structure see specific serialization method.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="buffer"></param>
/// <returns></returns>
template<typename T>
T Deserialize(char* buffer);

template<>
Common::Vote Deserialize(char* buffer);

template<>
Common::VotingOption Deserialize(char* buffer);

template<>
Common::VotingList Deserialize(char* buffer);

template<>
Common::VotesToCount Deserialize(char* buffer);

template<>
Common::CountedVotes Deserialize(char* buffer);

template<>
Common::VotesContainer Deserialize(char* buffer);

template<>
Common::FinalResult Deserialize(char* buffer);

#pragma region SubdataDeserialization

template<typename T>
T SubdataDeserialize(char* buffer, size_t& offset);

template<>
Common::Vote SubdataDeserialize(char* buffer, size_t& offset);

template<>
Common::VotingOption SubdataDeserialize(char* buffer, size_t& offset);

#pragma endregion
