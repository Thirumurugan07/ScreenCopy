#include <iostream>
#include "util/log.h"
#include "util/net.hpp"
#include "util/thread.h"
#include "comm.hpp"
#include "demuxer.hpp"
#include "packet_merger.hpp"
#include "util/binary.h"
using namespace std;


int main() {
    ScSocket socket;
    startServerAndConnect(socket);
    uint8_t data[4];
    ssize_t r = socket.recv_all(data, 4);
   
    uint32_t codec_id = sc_read32be(data);
    cout << codec_id << endl;
   /* ScDemuxer demuxer;
    demuxer.init("my-demuxer",socket);
    demuxer.start();
    demuxer.join();*/

    return 0;
}