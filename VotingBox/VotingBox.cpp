// VotingBox.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "VBThreadData/CounterThreadData.h"

#pragma comment(lib, "Ws2_32.lib")


#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>

#include <Common/Config.h>
#include <Common/CommonMethods.h>

#include <Serialization/Deserialization.h>
#include <ThreadPool/ThreadPool.h>

#include <Serialization/Serialization.h>
#include <ThreadPool/ThreadingMethods.h>
#include "AssembleCountedVotes/AssembleVotes.h"

#pragma warning(disable:4996)

UINT numberOfThreads = 10;

UINT numberOfVotingCounters;
std::time_t electionsEnd;
Common::VotingList votingOptionsGlobal;
AssembleVotes assembledVotes;

bool InitWindowsSockets();
void InitializeConfig();
void InitializeVotingOptions(Common::VotingList& votingList);

constexpr USHORT maxClientsConected = 10;
constexpr USHORT maxClientProcessed = 3;

DWORD WINAPI ProcessClient(LPVOID lplParam);
DWORD WINAPI ProcessVotingCounters(LPVOID lplParam);

Common::VotesContainer votesContainer;

void Elections(SOCKET listenSocket);
void CountVotes();
int SendFinalResults(const Common::FinalResult& results);

int main()
{
    InitializeConfig();
    InitializeVotingOptions(votingOptionsGlobal);

    SOCKET listenSocket = INVALID_SOCKET;

    int iResult = 0;

    if (!InitWindowsSockets()) {
        LogWinsockError("Error on initialization of winsock.", -1);
        return 1;
    }

    addrinfo* resultingAddress = NULL;
    addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4 address
    hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    hints.ai_flags = AI_PASSIVE;     // 

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, std::to_string(votingBoxPort).c_str(), &hints, &resultingAddress);
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
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket - bind port number and local address 
    // to socket
    iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Since we don't need resultingAddress any more, free it
    freeaddrinfo(resultingAddress);

    //// All connections are by default accepted by protocol stek if socket is in listening mode.
    //// With SO_CONDITIONAL_ACCEPT parameter set to true, connections will not be accepted by default
    bool bOptVal = true;
    int bOptLen = sizeof(bool);
    iResult = setsockopt(listenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (char*)&bOptVal, bOptLen);
    
    if (iResult == SOCKET_ERROR) {
        printf("setsockopt for SO_CONDITIONAL_ACCEPT failed with error: %u\n", WSAGetLastError());
    }

    unsigned long  mode = 1;
    if (ioctlsocket(listenSocket, FIONBIO, &mode) != 0) {
        printf("ioctlsocket failed with error.");
    }


    // Set listenSocket in listening mode
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server initialized, waiting for clients.\n");
    Elections(listenSocket);
    CountVotes();
    
    Common::FinalResult finalResult(assembledVotes.GetAssembledVotes(), votingOptionsGlobal.GetOptions());

    SendFinalResults(finalResult);

    //salji brojacima
    //primi prebrojane
    //salji inf ser - done

    // cleanup
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}

void Elections(SOCKET listenSocket)
{
    std::deque<SOCKET> acceptedSockets = {};

    std::deque<HANDLE> threadHandles = {};

    // set of socket descriptors
    fd_set readfds;

    // timeout for select function
    timeval timeVal;
    timeVal.tv_sec = 1;
    timeVal.tv_usec = 0;

    do
    {
        // initialize socket set
        FD_ZERO(&readfds);
        FD_SET(listenSocket, &readfds);

        // wait for events on set
        int selectResult = select(0, &readfds, NULL, NULL, &timeVal);

        if (selectResult == SOCKET_ERROR)
        {
            LogWinsockError("Select failed ", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            exit(1);
        }
        // timeout
        else if (selectResult == 0) {
            continue;
        }
        else if (FD_ISSET(listenSocket, &readfds)) {
            // Wait for clients and accept client connections.
                   // Returning value is acceptedSocket used for further
                   // Client<->Server communication. This version of
                   // server will handle only one client.
            SOCKET acceptedSocket = accept(listenSocket, NULL, NULL);

            acceptedSockets.push_back(acceptedSocket);

            if (acceptedSocket == INVALID_SOCKET)
            {
                LogWinsockError("accept failed.", WSAGetLastError());
                continue;
            }

            DWORD tId;

            HANDLE h = CreateThread(
                NULL, 0, ProcessClient, &acceptedSockets[acceptedSockets.size() - 1], 0, &tId
            );

            threadHandles.push_back(h);
        }

    } while (time(0) < electionsEnd);

    WaitCloseThreadHandles(&threadHandles);
}

#include <algorithm>

void CountVotes()
{
    auto splittedvotes = votesContainer.GetEquallySeparatedVotes((size_t)votingCountersNumber);

    // make this array
    std::vector<LPVOID> counterThreadData = {};
    
    std::deque<HANDLE> threadHandles = {};
    std::vector<SOCKET> connectSockets = {};

    for (short i = 0; i < votingCountersNumber; i++)
    {
        SOCKET connectSocket = INVALID_SOCKET;

        sockaddr_in counterAddress;
        counterAddress.sin_family = AF_INET;
        counterAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
        counterAddress.sin_port = htons(votingCountersFirstPort + i);

        connectSocket = socket(AF_INET,
            SOCK_STREAM,
            IPPROTO_TCP);

        if (connectSocket == INVALID_SOCKET)
        {
            LogWinsockError("socket failed.", WSAGetLastError());
            closesocket(connectSocket);
            continue;
        }

        // connect to server specified in serverAddress and socket connectSocket
        if (connect(connectSocket, (SOCKADDR*)&counterAddress, sizeof(counterAddress)) == SOCKET_ERROR)
        {
            LogWinsockError("Unable to connect to counter", WSAGetLastError());
            closesocket(connectSocket);
            continue;
        }

        CounterThreadData* ctdp = new CounterThreadData(
            connectSocket, 
            Common::VotesToCount(
                splittedvotes[i], votingOptionsGlobal.GetOptions()
            )
        );

        counterThreadData.push_back(
            ctdp
        );

        connectSockets.push_back(connectSocket);
    }

    for (short i = 0; i  < votingCountersNumber; i ++)
    {
        threadHandles.push_back(
            CreateThread(
                NULL, 0, ProcessVotingCounters, counterThreadData[i], 0, 0
            )
        );
    }

    WaitCloseThreadHandles(&threadHandles);

    std::for_each(
        counterThreadData.begin(),
        counterThreadData.end(),
        [](LPVOID lp) {
            CounterThreadData* ctd = ((CounterThreadData*)lp);
            delete ctd;
        }
    );

    counterThreadData.clear();
}

// note: lower default buffer size
DWORD __stdcall ProcessVotingCounters(LPVOID lplParam)
{
    CounterThreadData* ctd = ((CounterThreadData*)lplParam);
    SOCKET connectSocket = ctd->ConnectSocket;
    Common::VotesToCount votesToCount = ctd->VotesToCount;

    int iResult = 0;

    char recvBuff[defaultBufferLength];
    
    char* message = Serialize(votesToCount);

    iResult = send(
        connectSocket,
        message,
        (int)votesToCount.BufferSize(),
        0
    );

    delete[] message;

    if (iResult == SOCKET_ERROR)
    {
        LogWinsockError("send failed", WSAGetLastError());
        closesocket(connectSocket);
        return 1;
    }

    iResult = recv(
        connectSocket,
        recvBuff,
        defaultBufferLength,
        0
    );

    if (iResult == SOCKET_ERROR)
    {
        LogWinsockError("recv failed", WSAGetLastError());
        closesocket(connectSocket);
        return 1;
    }
    
    Common::CountedVotes countedVotes = Deserialize<Common::CountedVotes>(recvBuff);

    assembledVotes.AddVotes(countedVotes.GetCountedVotes());

    shutdown(connectSocket, SD_BOTH);
    closesocket(connectSocket);

    return 0;
}

#include <algorithm>

DWORD __stdcall ProcessClient(LPVOID lplParam)
{
    SOCKET accSocket = *((SOCKET*)lplParam);
    
    char recvBuff[defaultBufferLength];
    int iResult;
    int iSelectResult = 0;
    ULONG mode = 0;

    auto LambdaOnFinishThread = [&](SOCKET socket) {
        shutdown(socket, SD_BOTH);
        closesocket(socket);
    };

    if (iSelectResult = ioctlsocket(
        accSocket, FIONBIO, &mode
    ) != 0) {
        LogWinsockError("Error when setting accepted socket to be non-blocking.", WSAGetLastError());
        LambdaOnFinishThread(accSocket);
        return 1;
    }   

    timeval timeVal;
    timeVal.tv_sec = 0;
    timeVal.tv_usec = (long)pow(10, 5); // 100ms

    fd_set readfds;
    fd_set writefds;


    UINT maxNumOfTries = 500; // 500 * 100ms = 50 000ms = 50s
    UINT numOfTries = 0;

    // send options non blocking
    while (true)
    {
        FD_ZERO(&writefds);
        FD_SET(accSocket, &writefds);

        iSelectResult = select(0, NULL, &writefds, NULL, &timeVal);

        if (iSelectResult == SOCKET_ERROR) {
            LogWinsockError(
                "Send failed when processing client.",
                WSAGetLastError()
            );
            LambdaOnFinishThread(accSocket);
        }
        // timeout
        else if (iSelectResult == 0) {
            if (++numOfTries == maxNumOfTries){
                LambdaOnFinishThread(accSocket);
                return 1;
            }
            else {
                continue;
            }
        }
        else if (FD_ISSET(accSocket, &writefds)) {
            
            char* message = Serialize(votingOptionsGlobal);

            iResult = send(
                accSocket,
                message,
                (int)votingOptionsGlobal.BufferSize(),
                0
            );

            delete[] message;

            if (iResult == SOCKET_ERROR)
            {
                LogWinsockError("recv failed", WSAGetLastError());
                LambdaOnFinishThread(accSocket);
                return 1;
            }
            
            numOfTries = 0;
            break;
        }
        else {
            LogWinsockError("Undefined behaviour at send.", WSAGetLastError());
            LambdaOnFinishThread(accSocket);
            return 1;
        }
    }
    
    // receive vote non blocking
    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(accSocket, &readfds);

        iSelectResult = select(0, &readfds, NULL, NULL, &timeVal);

        if (iSelectResult == SOCKET_ERROR) {
            LogWinsockError(
                "Receive failed when processing client.",
                WSAGetLastError()
            );
            LambdaOnFinishThread(accSocket);
            return 1;
        }
        // timeout
        else if (iSelectResult == 0) {
            if (++numOfTries == maxNumOfTries){
                LambdaOnFinishThread(accSocket);
                return 1;
            }
            else {
                continue;
            }
        }
        else if (FD_ISSET(accSocket, &readfds)) {
            
            iResult = recv(
                accSocket,
                recvBuff,
                defaultBufferLength,
                0
            );

            if (iResult == SOCKET_ERROR)
            {
                LogWinsockError("recv failed", WSAGetLastError());
                LambdaOnFinishThread(accSocket);
                return 1;
            }

            Common::VotesContainer clientVoteContainer = Deserialize<Common::VotesContainer>(recvBuff);
            const std::deque<Common::Vote>& clientVotes = clientVoteContainer.GetVotes();

            std::for_each(
                clientVotes.begin(),
                clientVotes.end(),
                [&](const Common::Vote& v) {
                    votesContainer.AddVote(v);

                    std::cout << "Voter " << v.VoterId << " voted for "
                        << v.PartyNumber << " at " << ctime(&v.VoteTime) << std::endl;
                }
            );

            numOfTries = 0;
            break;
        }
        else {
            LogWinsockError("Undefined behaviour at receive.", WSAGetLastError());
            LambdaOnFinishThread(accSocket);
            return 1;
        }
    }
    
    LambdaOnFinishThread(accSocket);

    return 0;
}


int SendFinalResults(const Common::FinalResult& results) {
    SOCKET connectSocket = INVALID_SOCKET;
    // variable used to store function return value
    int iResult;
    // message to send
    // create a socket
    connectSocket = socket(AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(informationServicePort);//port informacionog servera
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    char* message = Serialize(results);

    iResult = send(
        connectSocket,
        message,
        (int)results.BufferSize(),
        0
    );

    delete[] message;

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    return 0;
}

#pragma warning(disable:4996)
#include <sstream>

void InitializeConfig()
{
    const std::string configNames[] = {
        "NUMBER OF VOTE COUNTERS",
        "ELECTIONS END"
    };

    std::wstring configPath = CurrentDirectoryPath() + L"\\votingBoxConfig.csv";

    std::ifstream file;

    file.open(
        configPath
    );

    ASSERT(file.is_open());

    std::map<std::string, std::string> configValues = {};
    std::string line;

    while (std::getline(file, line))
    {
        std::vector<std::string> splitted = SplitString(line, ',');

        ASSERT(splitted.size() == 2);

        TrimString(splitted[0]);
        TrimString(splitted[1]);

        configValues.insert({
            splitted[0], splitted[1]
            });
    }

    file.close();

    try
    {
        numberOfVotingCounters = std::stoi(
            configValues[configNames[0]]
        );
        
        std::string timeString = configValues[configNames[1]];
        struct std::tm tm;
        std::istringstream ss(timeString);

        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        
        electionsEnd = mktime(&tm);

        //auto check = ctime(&electionsEnd);
    }
    catch (const std::exception&)
    {
        std::cout << "Config file doesn't contain key." << std::endl;
        exit(1);
    }
}

void InitializeVotingOptions(Common::VotingList& votingList)
{
    std::wstring configPath = CurrentDirectoryPath() + L"\\votingOptions.csv";

    std::ifstream file;

    file.open(
        configPath
    );

    ASSERT(file.is_open());

    std::string line;

    std::int16_t brojac = 1;
    std::string partyLeader;
    std::string partyName;

    Common::VotingOption vOption;

    while (std::getline(file, line))
    {
        if (brojac % 4 != 0) {

            std::vector<std::string> splitted = SplitString(line, ',');
            TrimString(splitted[1]);

            if (brojac % 4 == 1) {
                partyName = splitted[1];
            }
            else
                if (brojac % 4 == 2) {
                    partyLeader = splitted[1];
                }
                else
                    if (brojac % 4 == 3) {
                        size_t optNum = stol(splitted[1], nullptr, 10);
                        votingList.AddOption(
                            Common::VotingOption(partyLeader, partyName, optNum)
                        );
                    }
        }
        brojac++;
    }

    file.close();
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