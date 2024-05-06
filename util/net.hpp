#pragma once

#include <iostream>
#include <cstdint>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

class ScSocket {
private:
    SOCKET socket_;
    bool closed_;

public:
    ScSocket();
    ~ScSocket();
    bool init();
    void cleanup();
    bool create();
    bool connect(uint32_t addr, uint16_t port);
    bool listen(uint32_t addr, uint16_t port, int backlog);
    ScSocket accept();
    ssize_t recv(void* buf, std::size_t len);
    ssize_t recv_all(void* buf, std::size_t len);
    /*ssize_t send(const void* buf, std::size_t len);
    ssize_t send_all(const void* buf, std::size_t len);*/
    //bool interrupt();
    bool close();

    // Public function to get the socket variable
    SOCKET getSocket() const { return socket_; }
};