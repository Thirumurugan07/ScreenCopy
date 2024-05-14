#include <iostream>
#include "util/log.h"
#include "util/net.hpp"
#include "util/thread.h"
#include "comm.hpp"
#include "demuxer.hpp"
#include "decoder.hpp"
#include "packet_merger.hpp"
#include "util/binary.h"
#include "trait/packet_source.hpp"
using namespace std;
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1234

int main() {
    ScSocket sockt;
    if (!sockt.start(SERVER_IP, SERVER_PORT)) {
        LOGE("socket creation failed");
    }

    ScDemuxer demuxer;
    ScDecoder decoder("my-decoder");
    demuxer.init("my-demuxer");
    demuxer.socket = sockt;
    
    decoder.init();
    sc_packet_source_add_sink(&demuxer.packet_source,&decoder.packet_sink);
    demuxer.start();
    struct sc_frame_source* src = &decoder.frame_source;
    demuxer.join();
    


    return 0;
}