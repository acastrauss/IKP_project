// Voter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define WIN32_LEAN_AND_MEAN

#include <iostream>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <Common/Config.h>
#include <Common/Vote.h>
#include <Common/VotingList.h>
#include <Common/VotingOption.h>
#include <Serialization/Serialization.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

bool InitializeWindowsSockets();


int main()
{
    SOCKET connectSocket = INVALID_SOCKET;
    // variable used to store function return value
    int iResult;
    // message to send
    
    Common::VotingOption opt1(
        "Pera peric",
        "qwerty",
        134
    );
    
    char* messageToSend = Serialize(opt1);

    if (InitializeWindowsSockets() == false)
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
    serverAddress.sin_port = htons(votingBoxPort);
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
    }

    // Send an prepared message with null terminator included
    iResult = send(connectSocket, messageToSend, opt1.BufferSize(), 0);
    delete[] messageToSend;

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // cleanup
    closesocket(connectSocket);
    WSACleanup();


    return 0;
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