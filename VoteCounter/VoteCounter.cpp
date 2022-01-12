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
#include <Serialization/Serialization.h>
#include <ThreadPool/ThreadPool.h>

#include <Common/CommonMethods.h>

#pragma comment(lib, "Ws2_32.lib")



int usedThreads = 0;
ULONG voterId = 0;

CRITICAL_SECTION cs;
CONDITION_VARIABLE cv;
struct ThreadCountVotes {
    std::deque<Common::Vote> Votes;
    Common::CountedVotes* Counted;

    explicit ThreadCountVotes(
        const std::deque<Common::Vote>& votes,
        Common::CountedVotes* counted
    ):
        Votes(votes), Counted(counted){}

    ThreadCountVotes(
        const ThreadCountVotes& ref
    ):
        Votes(ref.Votes), Counted(ref.Counted){};

    ThreadCountVotes() = delete;

    ~ThreadCountVotes() {
        Votes.clear();
    }
};


DWORD WINAPI Sum(LPVOID lpParam) {


    sockaddr_in serverAddress = *((sockaddr_in*)lpParam);

    // create a socket
    SOCKET connectSocket = socket(AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        //WSACleanup();
        return 1;
    }

    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");

        std::cout << WSAGetLastError() << std::endl;
        closesocket(connectSocket);
        //WSACleanup();
    }

    char buffer[defaultBufferLength];

    // Send an prepared message with null terminator included
    int iResult = recv(connectSocket, buffer, defaultBufferLength, 0);

    Common::VotesToCount votesToCount= Deserialize<Common::VotesToCount>(buffer);

    ThreadCountVotes* countVotes ; // ovo treba inicijalizovati
    countVotes->Votes = votesToCount.Votes;

    for (size_t i = 0; i < countVotes->Votes.size(); i++)
    {
        countVotes->Counted->IncreaseVoteNumber(
            countVotes->Votes[i].PartyNumber
        );
    }
    Common::CountedVotes countedV(*countVotes->Counted);
    char* voteMsg = Serialize(countedV);

    iResult = send(
        connectSocket,
        voteMsg,
        (int)countedV.BufferSize(),
        0
    );

    delete[] voteMsg;

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        //WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);
    shutdown(connectSocket, SD_BOTH);
    closesocket(connectSocket);

    Sleep(500);

    usedThreads--;
    WakeAllConditionVariable(&cv);

    
    return 0;
}

UINT splitNum = 3;

int main()
{
    {

        Common::CountedVotes countedVotesAll;

        Common::VotingOption opt1("Pera", "asdasd", 1);
        Common::VotingOption opt2("qweqwe", "asdasd", 2);
        Common::VotingOption opt3("fgfdg", "asdasd", 3);
        Common::VotingOption opt4("fgfdg", "asdasd", 4);
        Common::VotingList votingListReal({
            opt1, opt2, opt3, opt4
            });


        Common::Vote vote1(
            1, 1
        ); Common::Vote vote2(
            2, 1
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

        Common::VotesContainer toCount(Votes);
        auto splitted = toCount.GetEquallySeparatedVotes(splitNum);

        std::deque<HANDLE> threadHandles = {};
        std::deque<ThreadCountVotes> threadParams = {};


        for (UINT i = 0; i < splitNum; i++)
        {
            threadParams.push_back(ThreadCountVotes(splitted[i], &countedVotesAll));

            HANDLE h = CreateThread(
                NULL, 0, Sum, &threadParams[i], 0, 0
            );
        }


        while (threadHandles.size() != 0)
        {
            auto it = std::find_if(
                threadHandles.begin(),
                threadHandles.end(),
                [&](HANDLE h) {
                    if (
                        WaitForSingleObject(h, 1) == WAIT_OBJECT_0
                        ) {
                        CloseHandle(h);
                        return true;
                    }
                    else {
                        return false;
                    }
                }
            );

            if (it != threadHandles.end()) {
                threadHandles.erase(it);
            }
        }

        countedVotesAll.AsssignZeroVotes(votingListReal.GetOptions());
        threadHandles.clear();
        threadParams.clear();
    }
    // send

    return 0;
}



