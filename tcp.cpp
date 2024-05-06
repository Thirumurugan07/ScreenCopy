#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>


int connect() {
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        std::cerr << "Failed to initialize Winsock: " << result << std::endl;
        return 1;
    }
    SOCKET socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1234); // Change port as needed
    const wchar_t* ip_address = L"127.0.0.1";
    if (InetPton(AF_INET, ip_address, &server_addr.sin_addr) != 1) {
        std::cerr << "Invalid address format" << std::endl;
        closesocket(socket_fd);
        WSACleanup();
        return 1;
    }
    if (connect(socket_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server: " << WSAGetLastError() << std::endl;
        closesocket(socket_fd);
        WSACleanup();
        return 1;
    }
    std::cout << "Connected to the server" << std::endl;
}