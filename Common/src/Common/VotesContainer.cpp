#include "pch.h"
#include "Common/VotesContainer.h"
#include "Common/CommonMethods.h"
#include <algorithm>

namespace Common {
	VotesContainer::VotesContainer()
	{
		m_Votes = std::deque<Common::Vote>();
		InitializeCriticalSection(&cs);
	}
	
	VotesContainer::VotesContainer(const std::deque<Common::Vote> votes)
	{
		m_Votes = votes;
		InitializeCriticalSection(&cs);
	}

	VotesContainer::VotesContainer(const VotesContainer& rhs)
	{
		m_Votes = rhs.m_Votes;
		InitializeCriticalSection(&cs);
	}
	

	VotesContainer::~VotesContainer()
	{
		DeleteCriticalSection(&cs);
	}

	size_t Common::VotesContainer::BufferSize() const
	{
		EnterCriticalSection((LPCRITICAL_SECTION)&cs);

		size_t bufferSize = 0;
		bufferSize += sizeof(m_Votes.size());

		std::for_each(
			m_Votes.begin(),
			m_Votes.end(),
			[&](const Common::Vote vote) {
				size_t voteBufferSize = vote.BufferSize();
				bufferSize += sizeof(voteBufferSize);
				bufferSize += voteBufferSize;
			}
		);

		LeaveCriticalSection((LPCRITICAL_SECTION)&cs);

		return bufferSize;
	}

	/*VotesContainer& VotesContainer::operator=(const VotesContainer& rhs)
	{
		m_Votes = rhs.m_Votes;
		return *this;
	}*/

	std::deque<Common::Vote> VotesContainer::GetVotes() const
	{
		
		EnterCriticalSection((LPCRITICAL_SECTION)&cs);
		std::deque<Common::Vote> ret = m_Votes;
		LeaveCriticalSection((LPCRITICAL_SECTION)&cs);
		return ret;
	}

	std::vector<std::deque<Common::Vote>> VotesContainer::GetEquallySeparatedVotes(size_t parts) const
	{
		ASSERT(parts <= m_Votes.size());

		EnterCriticalSection((LPCRITICAL_SECTION)&cs);
		const std::deque<Common::Vote>& currentVotes = m_Votes;
		LeaveCriticalSection((LPCRITICAL_SECTION)&cs);

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

