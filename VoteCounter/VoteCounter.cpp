#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>

#include <Common/Config.h>
#include <Common/Vote.h>
#include <Common/VotingList.h>
#include <Common/VotingOption.h>
#include <Serialization/Deserialization.h>
#include <ThreadPool/ThreadPool.h>

#include <Common/CommonMethods.h>

#pragma comment(lib, "Ws2_32.lib")


Common::CountedVotes countVotesElections(std::deque<Common::Vote> countVotes) {
  std::deque<Common::Vote> Votes = countVotes;

  Common::CountedVotes counted;

  for (size_t i = 0; i < countVotes.size(); i++)
  {
    counted.IncreaseVoteNumber(countVotes[i].PartyNumber);
  }

  return counted;

}


int main()
{
  Common::Vote vote1(
    1, 1
  ); Common::Vote vote2(
    2, 3
  ); Common::Vote vote3(
    3, 4
  ); Common::Vote vote4(
    4, 2
  ); Common::Vote vote5(
    5, 2
  ); Common::Vote vote6(
    6, 4
  ); Common::Vote vote7(
    7, 1
  ); Common::Vote vote8(
    8, 1
  );

  std::deque<Common::Vote> Votes;
  Votes.push_back(vote1);
  Votes.push_back(vote2);
  Votes.push_back(vote3);
  Votes.push_back(vote4);
  Votes.push_back(vote5);
  Votes.push_back(vote6);
  Votes.push_back(vote7);
  Votes.push_back(vote8);



  Common::CountedVotes cnt = countVotesElections(Votes);
  std::int16_t a = 1;




}
struct ThreadCountVotes {
  std::deque<Common::Vote> Votes;
  Common::CountedVotes* counted;
};

/*
DWORD WINAPI Sum(LPVOID aaa) {

  ThreadCountVotes countVotes = ((ThreadCountVotes)aaa);

  std::deque<Common::Vote> Votes = countVotes.Votes;
  Common::CountedVotes* counted = countVotes.counted;

  for (size_t i = 0; i < countVotes.Votes.size(); i++)
  {
    counted->IncreaseVoteNumber(countVotes.Votes[i].PartyNumber);
  }

  return 0;
}*/
