#pragma once

#include <iostream>
#include <cstdint>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

typedef SOCKET sc_raw_socket;
#define SC_RAW_SOCKET_NONE INVALID_SOCKET

typedef struct sc_socket_windows {
    SOCKET socket;
    bool closed;
} sc_socket_windows;

typedef sc_socket_windows* sc_socket;

#define SC_SOCKET_NONE nullptr

bool net_init();
void net_cleanup();
sc_socket net_socket();
bool net_connect(sc_socket socket, uint32_t addr, uint16_t port);
bool net_listen(sc_socket server_socket, uint32_t addr, uint16_t port, int backlog);
sc_socket net_accept(sc_socket server_socket);
ssize_t net_recv(sc_socket socket, void* buf, std::size_t len);
ssize_t net_recv_all(sc_socket socket, void* buf, std::size_t len);
ssize_t net_send(sc_socket socket, const void* buf, std::size_t len);
ssize_t net_send_all(sc_socket socket, const void* buf, std::size_t len);
bool net_interrupt(sc_socket socket);
bool net_close(sc_socket socket);
bool net_parse_ipv4(const char* s, uint32_t* ipv4);