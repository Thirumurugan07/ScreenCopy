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
    if (!sockt.start(SERVER_IP, SERVER_PORT)) {
        LOGE("socket creation failed");
    }

    ScDemuxer demuxer;
    demuxer.init("my-demuxer");
    demuxer.socket = sockt;
    demuxer.start();
    demuxer.join();
    


    return 0;
}