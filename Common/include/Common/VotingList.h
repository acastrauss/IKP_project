#pragma once

#include "Common/VotingOption.h"
#include <vector>

namespace Common {

	typedef std::vector<VotingOption>::iterator vvoit;
	/// <summary>
	/// Class presenting voting options to client
	/// </summary>
	class VotingList
	{
	private:
		std::vector<VotingOption> m_Options;
	public:
		VotingList();
		explicit VotingList(
			const std::vector<VotingOption>& options
		);
		VotingList(
			const VotingList& ref
		);

		~VotingList();

		void AddOption(
			const VotingOption& option
		);

		vvoit FindOption(size_t optionNumber);

		void SortOptions();

		std::vector<VotingOption> GetOptions();
	};

}
