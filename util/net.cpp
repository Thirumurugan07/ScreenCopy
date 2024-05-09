#include "net.hpp"
#include <iostream>
#include "log.h"
#pragma comment(lib, "Ws2_32.lib")

ScSocket::ScSocket() : socket_(INVALID_SOCKET) {}

ScSocket::~ScSocket() { close(); }

bool ScSocket::init() {
    WSADATA wsa;
    int res = WSAStartup(MAKEWORD(1, 1), &wsa);
    if (res) {
        std::cerr << "WSAStartup failed with error " << res << std::endl;
        return false;
    }
    return true;
}

void ScSocket::cleanup() {
    WSACleanup();
}

bool ScSocket::create() {
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        return false;
    }
    return true;
}

bool ScSocket::connect(uint32_t addr, uint16_t port) {
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(addr);
    sin.sin_port = htons(port);

    if (::connect(socket_, (struct sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR) {
        std::cerr << "Error connecting to server" << std::endl;
        return false;
    }
    return true;
}

bool ScSocket::listen(uint32_t addr, uint16_t port, int backlog) {
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(addr);
    sin.sin_port = htons(port);

    if (bind(socket_, (struct sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR) {
        std::cerr << "Error binding socket" << std::endl;
        return false;
    }

    if (::listen(socket_, backlog) == SOCKET_ERROR) {
        std::cerr << "Error listening on socket" << std::endl;
        return false;
    }

    return true;
}

ScSocket ScSocket::accept() {
    SOCKET raw_sock = ::accept(socket_, NULL, NULL);
    if (raw_sock == INVALID_SOCKET) {
        std::cerr << "Error accepting connection" << std::endl;
        return ScSocket();
    }

    ScSocket client;
    client.socket_ = raw_sock;
    return client;
}

ssize_t ScSocket::recv(void* buf, std::size_t len) {
    return ::recv(socket_, static_cast<char*>(buf), len, 0);
}

ssize_t ScSocket::recv_all(void* buf, std::size_t len) {
    return ::recv(socket_, static_cast<char*>(buf), len, MSG_WAITALL);
}

bool ScSocket::close() {
    if (socket_ != INVALID_SOCKET) {
        if (::closesocket(socket_) == SOCKET_ERROR) {
            std::cerr << "Error closing socket" << std::endl;
            return false;
        }
        socket_ = INVALID_SOCKET;
    }
    return true;
}
bool ScSocket::net_parse_ipv4(const char* s, uint32_t* ipv4) {
    struct in_addr addr;
    if (!inet_pton(AF_INET, s, &addr)) {
        LOGE("Invalid IPv4 address: %s", s);
        return false;
    }

    *ipv4 = ntohl(addr.s_addr);
    return true;
}
bool ScSocket::start(const char* address, int port) {
    if (!init()) {
        std::cerr << "socket creation failed" << std::endl;
        cleanup();
        return false;
    }
    if (!create()) {
        std::cerr << "socket creation failed" << std::endl;
        cleanup();
        return false;
    }

    // Parse server IP address
    uint32_t server_addr;
    if (!net_parse_ipv4(address, &server_addr)) {
        std::cerr << "Failed to parse server IP address" << std::endl;
        cleanup();
        return false;
    }

    // Connect to the server
    if (!connect(server_addr, port)) {
        std::cerr << "Connection to server failed" << std::endl;
        cleanup();
        return false;
    }

    std::cout << "Server connected!" << std::endl;
    return true;
}
