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
#include <ThreadPool/ThreadingMethods.h>

#pragma comment(lib, "Ws2_32.lib")


constexpr UINT votingCounterBufferSize = 500000;

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
    
    ThreadCountVotes* countVotes = ((ThreadCountVotes*)lpParam);

    for (size_t i = 0; i < countVotes->Votes.size(); i++)
    {
        countVotes->Counted->IncreaseVoteNumber(
            countVotes->Votes[i].PartyNumber
        );
    }
    
    return 0;
}

UINT splitNum = 3;

bool InitWindowsSockets();

#pragma warning(disable:28193)

int main()
{
    Common::CountedVotes countedVotesAll;
    
    if (!InitWindowsSockets()) {
        LogWinsockError("Error on initialization of winsock.", -1);
        return 1;
    }

    unsigned short portToUse = votingCountersFirstPort;
    SOCKET listenSocket = INVALID_SOCKET;
    int iResult = 0;

    // so there can be more than one voting counter launched
    while (true)
    {
        listenSocket = INVALID_SOCKET;

        addrinfo* resultingAddress = NULL;
        addrinfo hints;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;       // IPv4 address
        hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
        hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
        hints.ai_flags = AI_PASSIVE;     // 

        // Resolve the server address and port
        iResult = getaddrinfo(NULL, std::to_string(portToUse).c_str(), &hints, &resultingAddress);

        if (iResult != 0)
        {
            printf("getaddrinfo failed with error: %d\n", iResult);
            WSACleanup();
            return 1;
        }

        // Create a SOCKET for connecting to server
        listenSocket = socket(AF_INET,      // IPv4 address famly
            SOCK_STREAM,  // stream socket
            IPPROTO_TCP); // TCP

        if (listenSocket == INVALID_SOCKET)
        {
            LogWinsockError("socket failed.", WSAGetLastError());
            freeaddrinfo(resultingAddress);
            WSACleanup();
            return 1;
        }

        iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            LogWinsockError("bind failed.", WSAGetLastError());
            freeaddrinfo(resultingAddress);
            
            if (++portToUse >= votingCountersFirstPort + votingCountersNumber) {
                std::cout << "Tried openning more than allowed number of counters" << std::endl;
                closesocket(listenSocket);
                WSACleanup();
                return 1;
            }
            // try again
            else {
                continue;
            }
        }
        // succesfull bind
        else {
            break;
        }
        // Since we don't need resultingAddress any more, free it
        freeaddrinfo(resultingAddress);
    }
    
    std::cout << "Using port:" << portToUse << std::endl;

    // Set listenSocket in listening mode
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    SOCKET acceptedSocket = accept(listenSocket, NULL, NULL);
    char buffer[votingCounterBufferSize];

    iResult = recv(
        acceptedSocket,
        buffer,
        votingCounterBufferSize,
        0
    );

    if (iResult == SOCKET_ERROR)
    {
        LogWinsockError("recv failed", WSAGetLastError());
        return 1;
    }

    std::cout << "Counter received:" << iResult << "B" << std::endl;

    Common::VotesToCount votesToCount = Deserialize<Common::VotesToCount>(buffer);

    // split votes for each thread
    Common::VotesContainer toCount(votesToCount.Votes);
    auto splitted = toCount.GetEquallySeparatedVotes(splitNum);

    std::deque<HANDLE> threadHandles = {};
    std::deque<ThreadCountVotes> threadParams = {};


    for (UINT i = 0; i < splitNum; i++)
    {
        threadParams.push_back(ThreadCountVotes(splitted[i], &countedVotesAll));

        HANDLE h = CreateThread(
            NULL, 0, Sum, &threadParams[i], 0, 0
        );

        threadHandles.push_back(h);
    }

    WaitCloseThreadHandles(&threadHandles);

    countedVotesAll.AsssignZeroVotes(votesToCount.Options);
    threadHandles.clear();
    threadParams.clear();
    // send

    char* voteMsg = Serialize(countedVotesAll);

    iResult = send(
        acceptedSocket,
        voteMsg,
        (int)countedVotesAll.BufferSize(),
        0
    );

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(acceptedSocket);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    delete[] voteMsg;

    //printf("Bytes Sent: %ld\n", iResult);
    shutdown(acceptedSocket, SD_BOTH);
    closesocket(acceptedSocket);
    shutdown(listenSocket, SD_BOTH);
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}


bool InitWindowsSockets()
{
    WSADATA wsaData;
    // Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
    return true;
}