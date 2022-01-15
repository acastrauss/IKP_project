#pragma once

#include <deque>
#include <Common/CountedVotes.h>

struct ThreadCountVotes {
    std::deque<Common::Vote> Votes;
    Common::CountedVotes* Counted;

    explicit ThreadCountVotes(
        const std::deque<Common::Vote>& votes,
        Common::CountedVotes* counted
    );

    ThreadCountVotes(
        const ThreadCountVotes& ref
    );

    ThreadCountVotes() = delete;

    ~ThreadCountVotes();
};
