#include "pch.h"
#include "Common/VotesContainer.h"
#include "Common/CommonMethods.h"

namespace Common {
	VotesContainer::VotesContainer()
	{
		m_Votes = std::deque<Common::Vote>();
		InitializeCriticalSection(&cs);
	}
	
	/*VotesContainer::VotesContainer(const VotesContainer& rhs)
	{
		m_Votes = rhs.m_Votes;
	}
	*/

	VotesContainer::~VotesContainer()
	{
		DeleteCriticalSection(&cs);
	}

	/*VotesContainer& VotesContainer::operator=(const VotesContainer& rhs)
	{
		m_Votes = rhs.m_Votes;
		return *this;
	}*/

	std::deque<Common::Vote> VotesContainer::GetVotes()
	{
		
		EnterCriticalSection(&cs);
		std::deque<Common::Vote> ret = m_Votes;
		LeaveCriticalSection(&cs);
		return ret;
	}

	std::vector<std::deque<Common::Vote>> VotesContainer::GetEquallySeparatedVotes(size_t parts)
	{
		ASSERT(parts <= m_Votes.size());

		EnterCriticalSection(&cs);
		const std::deque<Common::Vote>& currentVotes = m_Votes;
		LeaveCriticalSection(&cs);

		std::vector<std::deque<Common::Vote>> ret = {};

		size_t partSize = currentVotes.size() / parts;

		for (size_t i = 0; i < parts * partSize; i++)
		{
			if (i % partSize == 0) {
				ret.push_back({});
			}

			ret[ret.size() - 1].push_back(currentVotes[i]);
		}

		for (size_t i = parts * partSize; i < currentVotes.size(); i++)
		{
			ret[i % ret.size()].push_back(currentVotes[i]);
		}

		return ret;
	}

	void VotesContainer::AddVote(const Common::Vote& vote)
	{
		EnterCriticalSection(&cs);
		m_Votes.push_back(vote);
		LeaveCriticalSection(&cs);
	}
}

