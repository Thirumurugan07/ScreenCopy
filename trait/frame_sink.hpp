#pragma once

#include <cassert>
extern "C" {
#include <libavcodec/avcodec.h>

}
struct sc_frame_sink_ops {
    bool (*open)(struct sc_frame_sink* sink, const AVCodecContext* ctx);
    void (*close)(struct sc_frame_sink* sink);
    bool (*push)(struct sc_frame_sink* sink, const AVFrame* frame);
};

struct sc_frame_sink {
    const struct sc_frame_sink_ops* ops;
};
