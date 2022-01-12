// Voter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define WIN32_LEAN_AND_MEAN

#include <iostream>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <fstream>

#include <Common/Config.h>
#include <Common/Vote.h>
#include <Common/VotingList.h>
#include <Common/VotingOption.h>
#include <Serialization/Serialization.h>
#include <Serialization/Deserialization.h>
#include <Common/CommonMethods.h>
#include <ThreadPool/ThreadPool.h>

#pragma comment(lib, "Ws2_32.lib")

UINT numberOfVoters = 10;

void InitializeConfig();
bool InitializeWindowsSockets();
Common::Vote SelectVote(const Common::VotingList votingList);

DWORD WINAPI VotingProcess(LPVOID lpParam);

struct ThreadParams {
    sockaddr_in serverAddress;
    Common::ThreadPool* threadPool;
    DWORD threadId;
};

int usedThreads = 0;
ULONG voterId = 0;

CRITICAL_SECTION cs;
CONDITION_VARIABLE cv;

int main()
{
    InitializeConditionVariable(&cv);
    InitializeCriticalSection(&cs);

    std::vector<ThreadParams> connectSockets = {};
    std::vector<LPVOID> threadPoolParams = {};

    for (USHORT i = 0; i < maxVotersAtTime; i++)
    {
        connectSockets.push_back(ThreadParams());
        threadPoolParams.push_back(
            &connectSockets[i]
        );
    }

    Common::ThreadPool clientThreadPool(
        VotingProcess,
        threadPoolParams,
        maxVotersAtTime
    );

    int iResult;
    // message to send

    if (InitializeWindowsSockets() == false)
    {
        // we won't log anything since it will be logged
        // by InitializeWindowsSockets() function
        return 1;
    }

    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(votingBoxPort);

    std::deque<Common::ThreadInfo> threadInfoUsed = {};

    std::deque<HANDLE> threadHandles = {};


    while (
        numberOfVoters--
        ) {

        DWORD tId;

        while (usedThreads == 3) {
            SleepConditionVariableCS(&cv, &cs, INFINITE);
        }

        HANDLE h = CreateThread(
            NULL, 0, VotingProcess, &serverAddress, 0, &tId
        );

        usedThreads++;

        threadHandles.push_back(h);
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

    WSACleanup();

    DeleteCriticalSection(&cs);

    int a;
    std::cin >> a;

    return 0;
}


#include <random>

Common::Vote SelectVote(const Common::VotingList votingList)
{
    std::vector<Common::VotingOption> options = votingList.GetOptions();

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, (int)options.size() - 1); // define the range

    Common::Vote vote;
    vote.PartyNumber = options[distr(gen)].PartyNumber;
    vote.VoterId = ++voterId;

    return vote;
}

DWORD __stdcall VotingProcess(LPVOID lpParam)
{
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

    Common::VotingList votingList = Deserialize<Common::VotingList>(buffer);

    Common::Vote vote = SelectVote(votingList);

    char* voteMsg = Serialize(vote);

    iResult = send(
        connectSocket,
        voteMsg,
        (int)vote.BufferSize(),
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

void InitializeConfig()
{
    const std::string configNames[] = {
        "NUMBER OF VOTERS"
    };

    std::wstring configPath = CurrentDirectoryPath() + L"\\voterConfig.csv";

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
        numberOfVoters = std::stoi(
            configValues[configNames[0]]
        );
    }
    catch (const std::exception&)
    {
        std::cout << "Config file doesn't contain key." << std::endl;
        exit(1);
    }
}

bool InitializeWindowsSockets()
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