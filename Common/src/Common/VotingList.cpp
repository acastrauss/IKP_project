#include "pch.h"
#include "Common/VotingList.h"
#include "Common/CommonMethods.h"
#include <iostream>
#include <algorithm>

namespace Common {
	VotingList::VotingList()
	{
		m_Options = std::vector<VotingOption>();
	}

	VotingList::VotingList(const std::vector<VotingOption>& options)
	{
		m_Options = options;
	}

	VotingList::VotingList(const VotingList& ref)
	{
		m_Options = ref.m_Options;
	}

	VotingList::~VotingList()
	{
		m_Options.clear();
	}

	void VotingList::AddOption(const VotingOption& option)
	{
		ASSERT(this->FindOption(option.PartyNumber) == m_Options.end());
		m_Options.push_back(option);
	}

	vvocit VotingList::FindOption(size_t optionNumber) const
	{
		return std::find_if(
			m_Options.begin(), m_Options.end(), [=](const VotingOption& vo) {
				return vo.PartyNumber == optionNumber;
			}
		);
	}

	size_t Common::VotingList::BufferSize() const
	{
		size_t bufferSize = 0;

		// meta data: number of options
		bufferSize += m_Options.size();

		// each option buffer size
		std::for_each(
			m_Options.begin(),
			m_Options.end(),
			[&](const Common::VotingOption option) {
				bufferSize += option.BufferSize();
			}
		);

		return bufferSize;
	}

	void VotingList::SortOptions()
	{
		std::sort(
			m_Options.begin(),
			m_Options.end(),
			[](const VotingOption& vo1, const VotingOption& vo2) {
				return vo1.PartyNumber < vo2.PartyNumber;
			}
		);
	}

	std::vector<VotingOption> VotingList::GetOptions() const
	{
		return m_Options;
	}
}

