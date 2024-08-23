#include <iostream>
#include <WinSock2.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSASTARTUP FAILED :" << iResult << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server is listening on port 12345..." << endl;

    char buffer[1024];
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    while (true) {
        int recvLen = recvfrom(serverSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &clientAddrSize);
        if (recvLen == SOCKET_ERROR) {
            cout << "Receive failed: " << WSAGetLastError() << endl;
            continue;
        }

        buffer[recvLen] = '\0'; // Null-terminate the received data
        cout << "Received message: " << buffer << endl;

        // Optionally send a response to the client
        const char* response = "Message received";
        sendto(serverSocket, response, strlen(response), 0, (sockaddr*)&clientAddr, clientAddrSize);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
