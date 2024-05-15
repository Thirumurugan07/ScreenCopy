// sc_stream.h
#pragma once
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
    struct SwsContext;
}
extern "C++" {
#include <opencv2/opencv.hpp>
}
#include "trait/frame_sink.hpp"

class ScStream {
public:
    ScStream();
    ~ScStream();

    void init();

    AVFrame* av_frame;
    cv::Mat cv_frame;
    AVCodecContext* codec_ctx;
    struct sc_frame_sink frame_sink;

private:
    static bool frame_sink_open(struct sc_frame_sink* sink, const AVCodecContext* ctx);
    static void frame_sink_close(struct sc_frame_sink* sink);
    static bool frame_sink_push(struct sc_frame_sink* sink, const AVFrame* frame);

    static const struct sc_frame_sink_ops frame_sink_ops;
};