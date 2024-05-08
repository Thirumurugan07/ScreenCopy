#ifndef SC_STREAMER_H
#define SC_STREAMER_H
#include "common.hpp"
#include "streamer_source.h"
#include "trait/frame_sink.h"

#include <stdbool.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

struct sc_streamer {
    struct sc_frame_sink frame_sink ; // packet sink trait
    struct sc_streamer_source streamer_source; // frame source trait

    const char* name; // must be statically allocated (e.g. a string literal)

    AVCodecContext* ctx;
    AVFrame* frame;
};

// The name must be statically allocated (e.g. a string literal)
void
sc_decoder_init(struct sc_decoder* decoder, const char* name);

#endif