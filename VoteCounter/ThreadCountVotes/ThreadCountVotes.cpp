
#include "ThreadCountVotes.h"

ThreadCountVotes::ThreadCountVotes(
	const std::deque<Common::Vote>& votes,
	Common::CountedVotes* counted
):
	Votes(votes), Counted(counted) 
{}

ThreadCountVotes::ThreadCountVotes(const ThreadCountVotes& ref):
	Votes(ref.Votes), Counted(ref.Counted) 
{};

ThreadCountVotes::~ThreadCountVotes()
{
	Votes.clear();
}