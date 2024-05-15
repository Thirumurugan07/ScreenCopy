#include "decoder.hpp"



ScDecoder::ScDecoder(const char* name) : name(name), ctx(nullptr), frame(nullptr) {}



/** Downcast packet_sink to decoder */
#define DOWNCAST(SINK) container_of(SINK, ScDecoder, packet_sink)

static bool
sc_decoder_open(ScDecoder decoder, AVCodecContext* ctx) {
    decoder.frame = av_frame_alloc();
    if (!decoder.frame) {
        LOG_OOM();
        return false;
    }

    if (!sc_frame_source_sinks_open(&decoder.frame_source, ctx)) {
        av_frame_free(&decoder.frame);
        return false;
    }

    decoder.ctx = ctx;

    return true;
}


static void
sc_decoder_close(ScDecoder decoder) {
    sc_frame_source_sinks_close(&decoder.frame_source);
    av_frame_free(&decoder.frame);
}

static bool sc_decoder_push(ScDecoder* decoder, const AVPacket* packet) {
    bool is_config = packet->pts == AV_NOPTS_VALUE;
    if (is_config) {
        // nothing to do
        return true;
    }

    int ret = avcodec_send_packet(decoder->ctx, packet);
    if (ret < 0 && ret != AVERROR(EAGAIN)) {
        LOGE("Decoder '%s': could not send video packet: %d",
            decoder->name, ret);
        return false;
    }
 

    // Create a SwsContext for the conversion
    struct SwsContext* swsctx = sws_getContext(1080, 2400, AV_PIX_FMT_YUV420P,
        1080, 2400, AV_PIX_FMT_BGR24,
        SWS_BICUBIC, NULL, NULL, NULL);

    if (!swsctx) {
        LOGE("Failed to allocate SwsContext");
        // Handle the error appropriately, e.g., return false
    }

    for (;;) {
        ret = avcodec_receive_frame(decoder->ctx, decoder->frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        }

        if (ret) {
            LOGE("Decoder '%s', could not receive video frame: %d",
                decoder->name, ret);
            return false;
        }

        int dst_width = decoder->frame->width;
        int dst_height = decoder->frame->height;
        int dst_channels = 3; // BGR format
        int bgr_size = av_image_get_buffer_size(AV_PIX_FMT_BGR24, dst_width, dst_height, 1);
        uint8_t* bgr_buffer = (uint8_t*)av_malloc(bgr_size);
        if (!bgr_buffer) {
            LOGE("Failed to allocate buffer for BGR image");
            // Handle the error appropriately, e.g., return false
        }

        // Create AVFrame for BGR image
        AVFrame* bgr_frame = av_frame_alloc();
        if (!bgr_frame) {
            LOGE("Failed to allocate AVFrame for BGR image");
            av_free(bgr_buffer);
        }
        av_image_fill_arrays(bgr_frame->data, bgr_frame->linesize, bgr_buffer, AV_PIX_FMT_BGR24, dst_width, dst_height, 1);

        // Perform pixel format conversion
        sws_scale(swsctx, decoder->frame->data, decoder->frame->linesize, 0, decoder->frame->height,
            bgr_frame->data, bgr_frame->linesize);

   

        // Push the frame to the frame source
      //  bool ok = sc_frame_source_sinks_push(&decoder->frame_source, bgr_frame);
        av_frame_unref(decoder->frame);
        av_frame_free(&bgr_frame);
        av_free(bgr_buffer);
        //if (!ok) {
        //    // Error already logged
        //    return false;
        //}
    }

    // Free the SwsContext
    if (swsctx) {
        sws_freeContext(swsctx);
    }

    return true;
}


static bool
sc_decoder_packet_sink_open(struct sc_packet_sink* sink, AVCodecContext* ctx) {
    ScDecoder* decoder = DOWNCAST(sink);
    return sc_decoder_open(*decoder, ctx);
}

static void
sc_decoder_packet_sink_close(struct sc_packet_sink* sink) {
    ScDecoder* decoder = DOWNCAST(sink);
    sc_decoder_close(*decoder);
}

static bool
sc_decoder_packet_sink_push(struct sc_packet_sink* sink,
    const AVPacket* packet) {
    ScDecoder* decoder = DOWNCAST(sink);
    return sc_decoder_push(decoder, packet);
}

void ScDecoder::init() {
    sc_frame_source_init(&frame_source);
    assert(&frame_source);

    static const struct sc_packet_sink_ops ops = {
        sc_decoder_packet_sink_open,   
        sc_decoder_packet_sink_close,  
        sc_decoder_packet_sink_push    
    };

    packet_sink.ops = &ops;
}