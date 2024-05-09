#ifndef SC_STREAMER_H
#define SC_STREAMER_H
#include "common.hpp"

#include "trait/frame_sink.h"
#include <stdbool.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

struct sc_streamer {
    struct sc_frame_sink* frame_sink ; // packet sink trait

    const char* name; // must be statically allocated (e.g. a string literal)

    AVCodecContext* ctx;
    AVFrame* frame;
    const char* window_name;
    // OpenCV window display flag
    bool display_window;
};

// The name must be statically allocated (e.g. a string literal)
void
sc_streamer_init(struct sc_streamer* streamer, const char* name);


// Method to display frames using OpenCV
void sc_streamer_display_frames(struct sc_streamer* streamer);


#endif