#include "streamer.hpp"
#include "util/log.h"
#include "trait/frame_sink.hpp"
#include "trait/frame_source.hpp"
#define DOWNCAST(SINK) container_of(SINK, ScStream, frame_sink)


static bool
sc_stream_open(ScStream stream, AVCodecContext* ctx) {
    stream.av_frame = av_frame_alloc();
    if (!stream.av_frame) {
        LOG_OOM();
        return false;
    }

    /* if (!sc_frame_source_sinks_open(&stream.frame_source, ctx)) {
         av_frame_free(&stream.frame);
         return false;
     }*/

    // stream.ctx = ctx;

    return true;
}


static void
sc_stream_close(ScStream stream) {
   // sc_frame_source_sinks_close(&stream.frame_source);
    av_frame_free(&stream.av_frame);
}


static bool
sc_stream_frame_sink_open(struct sc_frame_sink* sink, AVCodecContext* ctx) {
    ScStream* stream = DOWNCAST(sink);
    return sc_stream_open(*stream, ctx);
}

static void
sc_stream_frame_sink_close(struct sc_frame_sink* sink) {
    ScStream* stream = DOWNCAST(sink);
    sc_stream_close(*stream);
}

static bool
sc_stream_frame_sink_push(struct sc_frame_sink* sink,
    const AVFrame* frame) {
    ScStream* stream = DOWNCAST(sink);
    return sc_stream_push(stream, frame);
}

void ScStream::init() {
    // Initialize the frame sink
    av_frame = av_frame_alloc();
    if (!av_frame) {
        LOG_OOM();
    }
    static const struct sc_frame_sink_ops ops = {
    sc_stream_frame_sink_open,
    sc_stream_frame_sink_close,
    sc_stream_frame_sink_push,
    };

    frame_sink.ops = &ops;
}