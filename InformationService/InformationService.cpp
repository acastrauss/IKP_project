// InformationService.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <ws2tcpip.h>

#include <algorithm>
#include <Common/Config.h>
#include <Serialization/Deserialization.h>

#include <Common/CommonMethods.h>

#pragma comment(lib, "Ws2_32.lib")

constexpr const size_t informationServiceBuffer = 16000;

bool InitializeWindowsSockets();

int main()
{
    std::cout << "Press any key to start" << std::endl;
    std::getchar();

    {
        SOCKET listenSocket = INVALID_SOCKET;
        SOCKET acceptedSocket = INVALID_SOCKET;
        int iResult = 0;
        // Buffer used for storing incoming data
        char recvbuf[informationServiceBuffer];

        if (InitializeWindowsSockets() == false)
        {
            return 1;
        }

        // Prepare address information structures
        addrinfo* resultingAddress = NULL;
        addrinfo hints;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;       // IPv4 address
        hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
        hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
        hints.ai_flags = AI_PASSIVE;     // 

        // Resolve the server address and port
        iResult = getaddrinfo(NULL, std::to_string(informationServicePort).c_str(), &hints, &resultingAddress);
        if (iResult != 0)
        {
            LogWinsockError("getaddrinfo failed ", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Create a SOCKET for connecting to server
        listenSocket = socket(AF_INET,      // IPv4 address famly
            SOCK_STREAM,  // stream socket
            IPPROTO_TCP); // TCP

        if (listenSocket == INVALID_SOCKET)
        {
            LogWinsockError("socket failed", WSAGetLastError());
            freeaddrinfo(resultingAddress);
            WSACleanup();
            return 1;
        }

        // Setup the TCP listening socket - bind port number and local address 
        // to socket
        iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            LogWinsockError("bind failed", WSAGetLastError());
            freeaddrinfo(resultingAddress);
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        // Since we don't need resultingAddress any more, free it
        freeaddrinfo(resultingAddress);

        // Set listenSocket in listening mode
        iResult = listen(listenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR)
        {
            LogWinsockError("listen failed", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        printf("Final server initialized.\n");
        // Wait for clients and accept client connections.
        // Returning value is acceptedSocket used for further
        // Client<->Server communication. This version of
        // server will handle only one client.
        acceptedSocket = accept(listenSocket, NULL, NULL);

        if (acceptedSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        // Receive data until the client shuts down the connection
        iResult = recv(acceptedSocket, recvbuf, informationServiceBuffer, 0);
        if (iResult > 0)
        {
            // printf("Message received from voting box: %s.\n", recvbuf);

            Common::FinalResult results = Deserialize<Common::FinalResult>(recvbuf);
            
            size_t countsNumber = 0;
            for (size_t o = 0; o < results.Results.size(); o++)
            {
                countsNumber += results.Results[o].second;
            }
            
            std::sort(
                results.Results.begin(),
                results.Results.end(),
                [](const auto& pair1, const auto& pair2) {
                    return pair1.second > pair2.second;
                }
            );

            std::cout << "Elections results : " << std::endl;
            std::cout << "----------------------------------------------------------------------" << std::endl;
            for (size_t i = 0; i < results.Results.size(); i++)
            {

                std::cout << i + 1 << '.' << results.Results[i].first.PartyName << ' ' << results.Results[i].second << std::endl;
                int percentage = (int)((results.Results[i].second * 1.0 / countsNumber) * 100) / 1;

                for (size_t j = 0; j < percentage; j++)
                {
                    std::cout << '*';
                }
                std::cout << std::endl;
            }
        }
        else if (iResult == 0)
        {
            // connection was closed gracefully
            printf("Connection with voting box closed.\n");
            closesocket(acceptedSocket);
        }
        else
        {
            // there was an error during recv
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(acceptedSocket);
        }

        // here is where server shutdown loguc could be placed
        // shutdown the connection since we're done
        iResult = shutdown(acceptedSocket, SD_SEND);
        if (iResult == SOCKET_ERROR)
        {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(acceptedSocket);
            WSACleanup();
            return 1;
        }

        // cleanup
        closesocket(listenSocket);
        closesocket(acceptedSocket);
        WSACleanup();
    }

    std::cout << "Press any key to exit" << std::endl;
    std::getchar();

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

