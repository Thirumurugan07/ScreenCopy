#include <iostream>
#include "util/log.h"
#include "util/net.hpp"
using namespace std;


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1234
int main() {
	system("start cmd /c C:\\Users\\HIZ1COB\\Developer\\scrcpy\\vlc.bat");
    Sleep(4000);
    ScSocket socket;
    if (!socket.init()) {
        std::cerr << "Socket creation failed" << std::endl;
        socket.cleanup();
        exit(EXIT_FAILURE);
    }

    // Parse server IP address
    uint32_t server_addr;
    if (!socket.net_parse_ipv4(SERVER_IP, &server_addr)) {
        std::cerr << "Failed to parse server IP address" << std::endl;
        socket.cleanup();
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (!socket.connect(server_addr, SERVER_PORT)) {
        std::cerr << "Connection to server failed" << std::endl;
        socket.cleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Server connected!" << endl;
    return 0;
}