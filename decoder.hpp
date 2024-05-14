#pragma once
#include "common.hpp"

#include <cstdbool>
#include "trait/frame_source.hpp"
#include "trait/packet_sink.hpp"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswscale/swscale.h>
#include "util/log.h"

#include "events.hpp"

}
extern "C" {
    struct AVCodecContext;
    struct AVFormatContext;
    struct sc_packet_sink;
    struct sc_frame_source;
    struct AVPacket;
}
class ScDecoder {
public:
    ScDecoder(const char* name);

    void init();
     
        struct sc_packet_sink packet_sink;
        struct sc_frame_source frame_source;

        AVCodecContext* ctx;
        AVFrame* frame;
    
    const char* name;

    
};
