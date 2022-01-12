// VotingBox.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "SocketPool/AcceptedSocketPool.h"
#include "ProcessClientData/ProcessClientData.h"
#include "ProcessClientData/ProcessClientWrapData.h"
#include <WS2tcpip.h>
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

#define DEFAULT_PORT_IS 33333
#pragma warning(disable:4996)

UINT numberOfThreads = 10;

UINT numberOfVotingCounters;
std::time_t electionsEnd;
std::vector<Common::VotingOption> votingOptionsGlobal;

bool InitWindowsSockets();
void InitializeConfig();
void InitializeVotingOptions();

constexpr USHORT maxClientsConected = 10;
constexpr USHORT maxClientProcessed = 3;

DWORD WINAPI ProcessClient(LPVOID lplParam);
DWORD WINAPI ProcessVotingCounters(LPVOID lplParam);

Common::VotesContainer votesContainer;
Common::VotingList votingListReal;
Common::VotesToCount votesToCount;
struct ClientThreadData {
    SOCKET acceptedSocket;
    Common::ThreadPool* threadPool;
    DWORD threadId;
};



int sendFinalResults(Common::FinalResult results);
int main()
{
   /* Common::VotingOption opt1("Pera", "asdasd", 1);
    Common::VotingOption opt2("qweqwe", "asdasd", 2);
    Common::VotingOption opt3("fgfdg", "asdasd", 3);

    votingListReal.AddOption(opt1);
    votingListReal.AddOption(opt2);
    votingListReal.AddOption(opt3);*/

    //std::vector<ClientThreadData> clientThreadData = {};
    //std::vector<LPVOID> clientThreadParams = {};

    //for (USHORT i = 0; i < maxClientProcessed; i++)
    //{
    //    clientThreadData.push_back(
    //        ClientThreadData()
    //    );

    //    clientThreadParams.push_back(
    //        &clientThreadData[i]
    //    );
    //}

    //Common::ThreadPool clientProcessPool(
    //    ProcessClient,
    //    clientThreadParams,
    //    maxClientProcessed
    //);

    //SOCKET listenSocket = INVALID_SOCKET;

    //InitializeConfig();

    //int iResult = 0;

    //if (!InitWindowsSockets()) {
    //    return 1;
    //}

    //addrinfo* resultingAddress = NULL;
    //addrinfo hints;

    //memset(&hints, 0, sizeof(hints));
    //hints.ai_family = AF_INET;       // IPv4 address
    //hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
    //hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    //hints.ai_flags = AI_PASSIVE;     // 

    //// Resolve the server address and port
    //iResult = getaddrinfo(NULL, std::to_string(votingBoxPort).c_str(), &hints, &resultingAddress);
    //if (iResult != 0)
    //{
    //    printf("getaddrinfo failed with error: %d\n", iResult);
    //    WSACleanup();
    //    return 1;
    //}

    //// Create a SOCKET for connecting to server
    //listenSocket = socket(AF_INET,      // IPv4 address famly
    //    SOCK_STREAM,  // stream socket
    //    IPPROTO_TCP); // TCP

    //if (listenSocket == INVALID_SOCKET)
    //{
    //    printf("socket failed with error: %ld\n", WSAGetLastError());
    //    freeaddrinfo(resultingAddress);
    //    WSACleanup();
    //    return 1;
    //}

    //// Setup the TCP listening socket - bind port number and local address 
    //// to socket
    //iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    //if (iResult == SOCKET_ERROR)
    //{
    //    printf("bind failed with error: %d\n", WSAGetLastError());
    //    freeaddrinfo(resultingAddress);
    //    closesocket(listenSocket);
    //    WSACleanup();
    //    return 1;
    //}

    //// Since we don't need resultingAddress any more, free it
    //freeaddrinfo(resultingAddress);

    //// Set listenSocket in listening mode
    //iResult = listen(listenSocket, SOMAXCONN);
    //if (iResult == SOCKET_ERROR)
    //{
    //    printf("listen failed with error: %d\n", WSAGetLastError());
    //    closesocket(listenSocket);
    //    WSACleanup();
    //    return 1;
    //}

    //printf("Server initialized, waiting for clients.\n");
   
    //std::deque<SOCKET> acceptedSockets = {};

    //std::deque<HANDLE> threadHandles = {};

    //do
    //{
    //    // Wait for clients and accept client connections.
    //    // Returning value is acceptedSocket used for further
    //    // Client<->Server communication. This version of
    //    // server will handle only one client.
    //    SOCKET acceptedSocket = accept(listenSocket, NULL, NULL);

    //    acceptedSockets.push_back(acceptedSocket);

    //    if (acceptedSocket == INVALID_SOCKET)
    //    {
    //        printf("accept failed with error: %d\n", WSAGetLastError());
    //        /*closesocket(listenSocket);
    //        WSACleanup();
    //        return 1;*/
    //        continue;
    //    }

    //    DWORD tId;

    //    HANDLE h = CreateThread(
    //        NULL, 0, ProcessClient, &acceptedSockets[acceptedSockets.size() - 1], 0, &tId
    //    );

    //    threadHandles.push_back(h);

    //} while (time(0) < electionsEnd);
    ////} while (1);


    //while (threadHandles.size() != 0)
    //{
    //    auto it = std::find_if(
    //        threadHandles.begin(),
    //        threadHandles.end(),
    //        [&](HANDLE h) {
    //            if (
    //                WaitForSingleObject(h, 1) == WAIT_OBJECT_0
    //                ) {
    //                CloseHandle(h);
    //                return true;
    //            }
    //            else {
    //                return false;
    //            }
    //        }
    //    );

    //    if (it != threadHandles.end()) {
    //        threadHandles.erase(it);
    //    }
    //}

    // salji brojacima
    // primi prebrojane
    // salji inf ser - done



    std::map<size_t, size_t> countedVotes1 = { {1,100}, {2,500},{3,270} };

    Common::CountedVotes countedVotes(countedVotes1);
    Common::VotingOption opt1(
        "aca vucic",
        "lopov",
        1
    ); Common::VotingOption opt2(
        "dragan djilas",
        "najposteniji",
        2
    ); Common::VotingOption opt3(
        "Dragan Solak",
        "takodje posten",
        3
    );
    Common::FinalResult finalres(
        countedVotes1,
        { opt1,
        opt2,
        opt3
        }
    );

    int s = sendFinalResults(finalres);
    int c = 3;
    // cleanup
   // closesocket(listenSocket);
    
    //WSACleanup();

    return 0;
}


DWORD __stdcall ProcessVotingCounters(LPVOID lplParam)
{
    SOCKET accSocket = *((SOCKET*)lplParam);

    char recvBuff[defaultBufferLength];
    int iResult;
    votesToCount.Votes = votesContainer.GetVotes();
    votesToCount.Options =votingOptionsGlobal;
    char* message = Serialize(votesToCount);

    iResult = send(
        accSocket,
        message,
        (int)votesToCount.BufferSize(),
        0
    );

    delete[] message;

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        //closesocket(clientData->AcceptedSocket);
        //WSACleanup();
        return 1;
    }

    iResult = recv(
        accSocket,
        recvBuff,
        defaultBufferLength,
        0
    );

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        //closesocket(clientData->AcceptedSocket);
        //WSACleanup();
        return 1;
    }
    // sabrati sve vracene izbrojane glasove iz niti
  /*  Common::Vote vote = Deserialize<Common::Vote>(recvBuff);
    vote.VoteTime = time(0);

    votesContainer.AddVote(vote);

    std::cout << "Voter " << vote.VoterId << " voted for " << vote.PartyNumber << " at " << ctime(&vote.VoteTime) << std::endl;*/
   
    
    
    
    //nakon toga ovo pozvati 
    //sendFinalResults(finalres)

    shutdown(accSocket, SD_BOTH);
    closesocket(accSocket);

    return 0;
}


DWORD __stdcall ProcessClient(LPVOID lplParam)
{
    SOCKET accSocket = *((SOCKET*)lplParam);
    
    char recvBuff[defaultBufferLength];
    int iResult;

    char* message = Serialize(votingListReal);

    iResult = send(
        accSocket,
        message,
        (int)votingListReal.BufferSize(),
        0
    );

    delete[] message;

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        //closesocket(clientData->AcceptedSocket);
        //WSACleanup();
        return 1;
    }

    iResult = recv(
        accSocket,
        recvBuff,
        defaultBufferLength,
        0
    );

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        //closesocket(clientData->AcceptedSocket);
        //WSACleanup();
        return 1;
    }

    Common::Vote vote = Deserialize<Common::Vote>(recvBuff);
    vote.VoteTime = time(0);

    votesContainer.AddVote(vote);
    
    std::cout << "Voter " << vote.VoterId << " voted for " << vote.PartyNumber << " at " << ctime(&vote.VoteTime) << std::endl;

    shutdown(accSocket, SD_BOTH);
    closesocket(accSocket);

    return 0;
}


int sendFinalResults(Common::FinalResult results) {
    SOCKET connectSocket = INVALID_SOCKET;
    // variable used to store function return value
    int iResult;
    // message to send

    
    if (InitWindowsSockets() == false)
    {
        // we won't log anything since it will be logged
        // by InitializeWindowsSockets() function
        return 1;
    }

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
    serverAddress.sin_port = htons(DEFAULT_PORT_IS);//port informacionog servera
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
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
        //closesocket(clientData->AcceptedSocket);
        //WSACleanup();
        return 1;
    }



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

void InitializeVotingOptions()
{

    std::wstring configPath = CurrentDirectoryPath() + L"\\votingOptions.csv";

    std::ifstream file;

    file.open(
        configPath
    );

    std::string line;

    Common::VotingList votingList;

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
                        vOption = Common::VotingOption(partyLeader, partyName, optNum);
                        votingList.AddOption(vOption);
                    }
        }
        brojac++;
    }

    file.close();
    votingOptionsGlobal = votingList.GetOptions();


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