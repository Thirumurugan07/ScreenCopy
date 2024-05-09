#ifndef SC_PACKET_MERGER_H
#define SC_PACKET_MERGER_H

#include "common.hpp"
#include <cstdint>
#include <cstddef>
extern "C" {
#include <libavcodec/avcodec.h>
}

class SCPacketMerger {
public:
    SCPacketMerger();
    ~SCPacketMerger();

    void init();
    void destroy();

    bool merge(AVPacket* packet);

private:
    uint8_t* config;
    size_t config_size;
};

#endif
