#ifndef SCSTREAM_HPP
#define SCSTREAM_HPP

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <opencv2/opencv.hpp>
#include "trait/frame_sink.h"

class ScStream {
public:
    ScStream();
    ~ScStream();

    void init();
   // void pullAndStreamFrame();
    AVFrame* av_frame;
    cv::Mat cv_frame;
    SwsContext* sws_ctx;
    struct sc_frame_sink frame_sink;

    cv::Mat avFrameToCvMat(const AVFrame* frame);
    void initializeSwsContext(AVCodecContext* codec_ctx);
};

#endif // SCSTREAM_HPP
