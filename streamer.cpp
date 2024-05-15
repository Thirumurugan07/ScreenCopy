#pragma once
#include "streamer.hpp"

#include "util/log.h"

#define DOWNCAST(SINK) container_of(SINK, ScStream, frame_sink)

const struct sc_frame_sink_ops ScStream::frame_sink_ops = {
    ScStream::frame_sink_open,
    ScStream::frame_sink_close,
    ScStream::frame_sink_push
};

ScStream::ScStream() : av_frame(nullptr), codec_ctx(nullptr) {
    frame_sink.ops = &frame_sink_ops;
}

ScStream::~ScStream() {
    if (av_frame) {
        av_frame_free(&av_frame);
    }
    if (codec_ctx) {
        avcodec_free_context(&codec_ctx);
    }
}

void ScStream::init() {
    av_frame = av_frame_alloc();
    if (!av_frame) {
        LOG_OOM();
    }
}

bool ScStream::frame_sink_open(struct sc_frame_sink* sink, const AVCodecContext* ctx) {
    ScStream* stream = DOWNCAST(sink);
    stream->codec_ctx = const_cast<AVCodecContext*>(ctx);
    return true;
}

void ScStream::frame_sink_close(struct sc_frame_sink* sink) {
    ScStream* stream = DOWNCAST(sink);
    // Clean up if needed
}

bool ScStream::frame_sink_push(struct sc_frame_sink* sink, const AVFrame* frame) {
    ScStream* stream = DOWNCAST(sink);
    stream->cv_frame = cv::Mat(frame->height, frame->width, CV_8UC3);

    struct SwsContext* sws_ctx = sws_getContext(
        frame->width, frame->height, (AVPixelFormat)frame->format,
        frame->width, frame->height, AV_PIX_FMT_BGR24,
        SWS_BILINEAR, NULL, NULL, NULL
    );

    uint8_t* dst_data[4] = { stream->cv_frame.data };
    int dst_linesize[4] = { stream->cv_frame.step1() };

    sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, dst_data, dst_linesize);
    sws_freeContext(sws_ctx);

    // Display the frame using OpenCV
    cv::imshow("Frame", stream->cv_frame);
    cv::waitKey(1);

    return true;
}