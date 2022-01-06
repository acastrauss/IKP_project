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



