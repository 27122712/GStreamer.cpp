#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed: " << iResult << endl;
        return 1;
    }

    // Create a UDP socket
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Define the server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);

    // Use a wide-character string for the IP address
    const wchar_t* serverIp = L"127.0.0.1";
    InetPtonW(AF_INET, serverIp, &serverAddr.sin_addr);

    // Send a message to the server
    const char* message = "Hello, UDP Server!";
    int sendResult = sendto(clientSocket, message, strlen(message), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (sendResult == SOCKET_ERROR) {
        cout << "Send failed: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Receive a response from the server
    char buffer[1024];
    sockaddr_in responseAddr;
    int responseAddrSize = sizeof(responseAddr);

    int recvLen = recvfrom(clientSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&responseAddr, &responseAddrSize);
    if (recvLen == SOCKET_ERROR) {
        cout << "Receive failed: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Null-terminate the received data and print it
    buffer[recvLen] = '\0';
    cout << "Received response: " << buffer << endl;

    // Clean up
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
