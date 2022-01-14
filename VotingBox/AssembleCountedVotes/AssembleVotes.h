#pragma once

#include <map>
#include <Windows.h>

/// <summary>
/// Used to assemble votes at voting box that are received from counters
/// </summary>
class AssembleVotes {

	/// <summary>
	/// Key: Party number, Value: Number of votes
	/// </summary>
	std::map<size_t, size_t> m_Votes;
	CRITICAL_SECTION cs;
public:
	AssembleVotes();
	~AssembleVotes();
	void AddVotes(std::map<size_t, size_t> votes);
	std::map<size_t, size_t> GetAssembledVotes();
};