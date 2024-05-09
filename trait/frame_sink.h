#pragma once

#include <cassert>
#include <libavcodec/avcodec.h>

struct sc_frame_sink {
    const struct sc_frame_sink_ops* ops;
};

struct sc_frame_sink_ops {
    /* The codec context is valid until the sink is closed */
    bool (*open)(struct sc_frame_sink* sink, const AVCodecContext* ctx);
    void (*close)(struct sc_frame_sink* sink);
    bool (*push)(struct sc_frame_sink* sink, const AVFrame* frame);
};
