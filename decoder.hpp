#pragma once
#include "common.hpp"
#include "trait/frame_source.h"
#include "trait/packet_sink.h"
#include <cstdbool>

extern "C" {


#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "events.h"

}

class ScDecoder {
public:
    ScDecoder(const char* name);

    void init();
  
    struct sc_packet_sink packet_sink;
    struct sc_frame_source frame_source;
    const char* name;
    AVCodecContext* ctx;
    AVFrame* frame;
    
};
