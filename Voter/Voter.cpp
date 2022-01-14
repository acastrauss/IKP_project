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
    UINT NumberOfVotesPerThread;
    const Common::VotingList& VotingList;
    Common::VotesContainer* VotesContainer;

    VotingThreadData() = delete;

    VotingThreadData(
        UINT numberOfVotesPerThread,
        const Common::VotingList& votingList,
        Common::VotesContainer* votesContainerP
    ) :
        NumberOfVotesPerThread(numberOfVotesPerThread),
        VotingList(votingList),
        VotesContainer(votesContainerP)
    {}

    VotingThreadData(
        const VotingThreadData& ref
    ): NumberOfVotesPerThread(ref.NumberOfVotesPerThread),
        VotingList(ref.VotingList),
        VotesContainer(ref.VotesContainer)
    {}

    VotingThreadData& operator=(
        const VotingThreadData& rhs
        ) = delete;
};

std::vector<UINT> EqualPartsNumberDivide(UINT number, UINT parts);

int usedThreads = 0;
DWORD voterId = 0;

VoterCV voterCV(maxVotersAtTime);

//CRITICAL_SECTION cs;
//CONDITION_VARIABLE cv;


int main()
{
    auto LambdaOnFinish = [&](SOCKET socket = INVALID_SOCKET) {
        if (socket != INVALID_SOCKET) {
            //shutdown(socket, SD_BOTH);
            closesocket(socket);
        }
        WSACleanup();
    };

    
    /*
    InitializeConditionVariable(&cv);
    InitializeCriticalSection(&cs);*/

    if (InitializeWindowsSockets() == false)
    {
        return 1;
    }
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(votingBoxPort);

    // create a socket
    SOCKET connectSocket = socket(AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        LogWinsockError("socket failed", WSAGetLastError());
        LambdaOnFinish();
        return 1;
    }

    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        LogWinsockError("Unable to connect to server.", WSAGetLastError());
        LambdaOnFinish(connectSocket);
        return 1;
    }

    char buffer[defaultBufferLength];

    // Send an prepared message with null terminator included
    int iResult = recv(connectSocket, buffer, defaultBufferLength, 0);

    if (iResult == SOCKET_ERROR || iResult == 0) {
        LogWinsockError("Did not receive options.", WSAGetLastError());
        LambdaOnFinish(connectSocket);
        return 1;
    }

    Common::VotingList votingList = Deserialize<Common::VotingList>(buffer);
    Common::VotesContainer votesContainer;

    std::vector<UINT> votesPerThread = EqualPartsNumberDivide(numberOfVoters, (UINT)voterThreads);

    std::vector<LPVOID> votingThreadParams = {};

    for (USHORT i = 0; i < voterThreads; i++)
    {
        VotingThreadData* vtdp = new VotingThreadData(
            votesPerThread[i],
            votingList,
            &votesContainer
        );

        votingThreadParams.push_back(vtdp);
    }

    for (size_t i = 0; i < votingThreadParams.size(); i++)
    {
        VotingThreadData* vtd = ((VotingThreadData*)votingThreadParams[i]);
        int a = 5;
    }

    Common::ThreadPool votingThreadPool(
        VotingProcess,
        votingThreadParams,
        voterThreads
    );

    USHORT clientThreadsNumber = 0;

    while (
        clientThreadsNumber < voterThreads
        ) {

        voterCV.StopIfMax();

        Common::ThreadInfo* tInfo = votingThreadPool.GetThreadBlocking();

        ResumeThread(tInfo->Handle);
        
        /*HANDLE h = CreateThread(
            NULL, 0, VotingProcess, &serverAddress, 0, &tId
        );*/

        voterCV.IncreaseVoters();

        clientThreadsNumber++;
        //threadHandles.push_back(h);
    }

    votingThreadPool.WaitForThreads();
    
    char* voteMsg = Serialize(votesContainer);

    iResult = send(
        connectSocket,
        voteMsg,
        (int)votesContainer.BufferSize(),
        0
    );

    delete[] voteMsg;

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        LambdaOnFinish(connectSocket);
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    for (size_t i = 0; i < votingThreadParams.size(); i++)
    {
        VotingThreadData* vtdp = ((VotingThreadData*)votingThreadParams[i]);
        if (vtdp) {
            delete vtdp;
        }
    }

    votingThreadParams.clear();

    LambdaOnFinish(connectSocket);

    //DeleteCriticalSection(&cs);

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
    VotingThreadData* vtd = ((VotingThreadData*)lpParam);
    UINT votesForThread = vtd->NumberOfVotesPerThread;
    Common::VotingList votingList(vtd->VotingList);
    Common::VotesContainer* vcp = vtd->VotesContainer;

    for (UINT i = 0; i < votesForThread; i++)
    {
        Common::Vote vote = SelectVote(votingList);
        vcp->AddVote(vote);
    }

    voterCV.VoterFinished();

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