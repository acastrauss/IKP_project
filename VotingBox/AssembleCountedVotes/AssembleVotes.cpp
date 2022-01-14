#include "AssembleVotes.h"

AssembleVotes::AssembleVotes()
: m_Votes()
{
	InitializeCriticalSection(&cs);
}

AssembleVotes::~AssembleVotes()
{
	m_Votes.clear();
	DeleteCriticalSection(&cs);
}

void AssembleVotes::AddVotes(std::map<size_t, size_t> votes)
{
	EnterCriticalSection(&cs);

	for (auto it = votes.begin(); it != votes.end(); it++)
	{
		if (m_Votes.find(it->first) != m_Votes.end()) {
			m_Votes[it->first] += it->second;
		}
		else {
			m_Votes.insert(*it);
		}
	}

	LeaveCriticalSection(&cs);
}

std::map<size_t, size_t> AssembleVotes::GetAssembledVotes()
{
	std::map<size_t, size_t> retVal = {};

	EnterCriticalSection(&cs);

	retVal = m_Votes;

	LeaveCriticalSection(&cs);

	return retVal;
}