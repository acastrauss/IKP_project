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
#include <ThreadPool/ThreadingMethods.h>
#include "VoterCV/VoterCV.h"

#pragma comment(lib, "Ws2_32.lib")

UINT numberOfVoters = 1000;
constexpr USHORT voterThreads = 11;

void InitializeConfig();
bool InitializeWindowsSockets();
Common::Vote SelectVote(const Common::VotingList votingList);

DWORD WINAPI VotingProcess(LPVOID lpParam);

struct VotingThreadData {
    sockaddr_in ServerAddress;
    UINT NumberOfVotesPerThread;
    
    VotingThreadData() = delete;

    VotingThreadData(
        sockaddr_in serverAddress,
        UINT numberOfVotesPerThread
    ) : ServerAddress(serverAddress),
        NumberOfVotesPerThread(numberOfVotesPerThread)
    {}

    VotingThreadData(
        const VotingThreadData& ref
    ): ServerAddress(ref.ServerAddress),
        NumberOfVotesPerThread(ref.NumberOfVotesPerThread)
    {}

    VotingThreadData& operator=(
        const VotingThreadData& rhs
        ) {
        ServerAddress = rhs.ServerAddress;
        NumberOfVotesPerThread = rhs.NumberOfVotesPerThread;
    }
};

std::vector<UINT> EqualPartsNumberDivide(UINT number, UINT parts);

int usedThreads = 0;
DWORD voterId = 0;

VoterCV voterCV(maxVotersAtTime);

//CRITICAL_SECTION cs;
//CONDITION_VARIABLE cv;

// create and initialize address structure
sockaddr_in serverAddress;
std::vector<UINT> votesPerThread;

int main()
{/*
    InitializeConditionVariable(&cv);
    InitializeCriticalSection(&cs);*/

    if (InitializeWindowsSockets() == false)
    {
        // we won't log anything since it will be logged
        // by InitializeWindowsSockets() function
        return 1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(votingBoxPort);

    votesPerThread = EqualPartsNumberDivide(numberOfVoters, (UINT)voterThreads);

    UINT votingThreadData[voterThreads];
    std::vector<LPVOID> votingThreadParams = {};

    for (size_t i = 0; i < votesPerThread.size(); i++)
    {
        votingThreadData[i] = votesPerThread[i];
        votingThreadParams.push_back(votingThreadData + i);
    }

    Common::ThreadPool votingThreadPool(
        VotingProcess,
        votingThreadParams,
        voterThreads
    );

    //std::deque<HANDLE> threadHandles = {};

    USHORT clientThreadsNumber = 0;

    while (
        clientThreadsNumber < voterThreads
        ) {

        voterCV.StopIfMax();

        Common::ThreadInfo* tInfo = votingThreadPool.GetThreadBlocking();
        //*((UINT*)tInfo->Parameter) = clientThreadsNumber;

        ResumeThread(tInfo->Handle);
        
        /*HANDLE h = CreateThread(
            NULL, 0, VotingProcess, &serverAddress, 0, &tId
        );*/

        voterCV.IncreaseVoters();

        clientThreadsNumber++;
        //threadHandles.push_back(h);
    }

    votingThreadPool.WaitForThreads();

    WSACleanup();

    //DeleteCriticalSection(&cs);

    int a;
    std::cin >> a;

    return 0;
}

std::vector<UINT> EqualPartsNumberDivide(UINT number, UINT parts)
{
    ASSERT(parts != 0);

    if (number < parts) {
        parts = number;
    }

    std::vector<UINT> partsRet = {};

    for (UINT i = 0; i < parts; i++)
    {
        partsRet.push_back(number / parts);
    }

    // unequal division
    if (
        partsRet.size() * partsRet[0] < number
        ) {
        UINT leftover = number - (UINT)partsRet.size() * partsRet[0];

        for (UINT i = 0; i < leftover; i++)
        {
            partsRet[i % partsRet.size()]++;
        }
    }

    return partsRet;
}

#include <random>
#pragma warning(disable:26451)

Common::Vote SelectVote(const Common::VotingList votingList)
{
    std::vector<Common::VotingOption> options = votingList.GetOptions();

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, (int)options.size() - 1); // define the range
    
    Common::Vote vote;
    vote.PartyNumber = options[distr(gen)].PartyNumber;
    vote.VoterId = (long long)(++voterId + GetCurrentProcessId());

    return vote;
}

DWORD __stdcall VotingProcess(LPVOID lpParam)
{
    UINT votesForThread = *((UINT*)lpParam);

    auto LambdaOnFinishThread = [&](bool closeSocket = false, SOCKET socket = INVALID_SOCKET) {
        if (closeSocket && socket != INVALID_SOCKET) {
            //shutdown(socket, SD_BOTH);
            closesocket(socket);
        }
        voterCV.VoterFinished();
    };

    // create a socket
    SOCKET connectSocket = socket(AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        //WSACleanup();
        LambdaOnFinishThread();
        return 1;
    }

    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        LogWinsockError("Unable to connect to server.", WSAGetLastError());
        LambdaOnFinishThread(true, connectSocket);
        return 1;
    }

    char buffer[defaultBufferLength];

    // Send an prepared message with null terminator included
    int iResult = recv(connectSocket, buffer, defaultBufferLength, 0);

    if (iResult == SOCKET_ERROR || iResult == 0) {
        
        LogWinsockError("Did not receive options.", WSAGetLastError());
        LambdaOnFinishThread(true, connectSocket);
        return 1;
    }

    Common::VotesContainer threadVotes;
    Common::VotingList votingList = Deserialize<Common::VotingList>(buffer);

    for (UINT i = 0; i < votesForThread; i++)
    {
        Common::Vote vote = SelectVote(votingList);
        threadVotes.AddVote(vote);
    }

    char* voteMsg = Serialize(threadVotes);

    iResult = send(
        connectSocket,
        voteMsg,
        (int)threadVotes.BufferSize(),
        0
    );

    delete[] voteMsg;

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        LambdaOnFinishThread(true, connectSocket);
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);
    LambdaOnFinishThread(true, connectSocket);

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