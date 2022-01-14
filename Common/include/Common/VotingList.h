#pragma once

#include "Common/VotingOption.h"
#include "Common/Serializable.h"
#include <vector>

namespace Common {

	typedef std::vector<VotingOption>::const_iterator vvocit;
	/// <summary>
	/// Class presenting voting options to client
	/// </summary>
	class VotingList : Serializable
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

		//~VotingList();

		void AddOption(
			const VotingOption& option
		);

		vvocit FindOption(size_t optionNumber) const;

		size_t BufferSize() const;

		void SortOptions();

		std::vector<VotingOption> GetOptions() const;
	};

}
