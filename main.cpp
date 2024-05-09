#include <iostream>
#include "util/log.h"
#include "util/net.hpp"
#include "util/thread.h"
#include "comm.hpp"
#include "demuxer.hpp"
#include "packet_merger.hpp"
#include "util/binary.h"
using namespace std;
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1234

int main() {
    ScSocket sockt;
    if (!sockt.init()) {
        std::cerr << "sockt creation failed" << std::endl;
        sockt.cleanup();
        exit(EXIT_FAILURE);
    }
    if (!sockt.create()) {
        std::cerr << "sockt creation failed" << std::endl;
        sockt.cleanup();
        exit(EXIT_FAILURE);
    }

    // Parse server IP address
    uint32_t server_addr;
    if (!sockt.net_parse_ipv4(SERVER_IP, &server_addr)) {
        std::cerr << "Failed to parse server IP address" << std::endl;
        sockt.cleanup();
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (!sockt.connect(server_addr, SERVER_PORT)) {
        std::cerr << "Connection to server failed" << std::endl;
        sockt.cleanup();
        exit(EXIT_FAILURE);
    }

    std::cout << "Server connected!" << std::endl;

    ScDemuxer demuxer;
    demuxer.init("my-demuxer");
    demuxer.socket = sockt;
    demuxer.start();
    demuxer.join();
    


    return 0;
}