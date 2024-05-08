#include "demuxer.hpp"
ScDemuxer::ScDemuxer() : name(nullptr), socket(ScSocket()), thread(sc_thread()) {
    // Initialize any other member variables if needed
}

enum AVCodecID ScDemuxer::to_avcodec_id(uint32_t codec_id) {
    #define SC_CODEC_ID_H264 UINT32_C(0x68323634) // "h264" in ASCII
    #define SC_CODEC_ID_H265 UINT32_C(0x68323635) // "h265" in ASCII
    #define SC_CODEC_ID_AV1 UINT32_C(0x00617631) // "av1" in ASCII
    #define SC_CODEC_ID_OPUS UINT32_C(0x6f707573) // "opus" in ASCII
    #define SC_CODEC_ID_AAC UINT32_C(0x00616163) // "aac" in ASCII
    #define SC_CODEC_ID_FLAC UINT32_C(0x666c6163) // "flac" in ASCII
    #define SC_CODEC_ID_RAW UINT32_C(0x00726177) // "raw" in ASCII
        switch (codec_id) {
        case SC_CODEC_ID_H264:
            return AV_CODEC_ID_H264;
        case SC_CODEC_ID_H265:
            return AV_CODEC_ID_HEVC;
        case SC_CODEC_ID_AV1:
    #ifdef SCRCPY_LAVC_HAS_AV1
            return AV_CODEC_ID_AV1;
    #else
            LOGE("AV1 not supported by this FFmpeg version");
            return AV_CODEC_ID_NONE;
    #endif
        case SC_CODEC_ID_OPUS:
            return AV_CODEC_ID_OPUS;
        case SC_CODEC_ID_AAC:
            return AV_CODEC_ID_AAC;
        case SC_CODEC_ID_FLAC:
            return AV_CODEC_ID_FLAC;
        case SC_CODEC_ID_RAW:
            return AV_CODEC_ID_PCM_S16LE;
        default:
            LOGE("Unknown codec id 0x%08" PRIx32, codec_id);
            return AV_CODEC_ID_NONE;
        }
}

bool ScDemuxer::recv_codec_id(uint32_t* codec_id) {
    uint8_t data[4];
    ssize_t r = socket.recv_all(data, 4);
    if (r < 4) {
        return false;
    }

    *codec_id = sc_read32be(data);
    return true;
}

bool ScDemuxer::recv_video_size(uint32_t* width, uint32_t* height) {

        uint8_t data[8];
        ssize_t r = socket.recv_all(data, 8);
        if (r < 8) {
            return false;
        }

        *width = sc_read32be(data);
        *height = sc_read32be(data + 4);
        return true;
}

bool ScDemuxer::recv_packet(AVPacket* packet) {
    // The video and audio streams contain a sequence of raw packets (as
   // provided by MediaCodec), each prefixed with a "meta" header.
   //
   // The "meta" header length is 12 bytes:
   // [. . . . . . . .|. . . .]. . . . . . . . . . . . . . . ...
   //  <-------------> <-----> <-----------------------------...
   //        PTS        packet        raw packet
   //                    size
   //
   // It is followed by <packet_size> bytes containing the packet/frame.
   //
   // The most significant bits of the PTS are used for packet flags:
   //
   //  byte 7   byte 6   byte 5   byte 4   byte 3   byte 2   byte 1   byte 0
   // CK...... ........ ........ ........ ........ ........ ........ ........
   // ^^<------------------------------------------------------------------->
   // ||                                PTS
   // | `- key frame
   //  `-- config packet

    uint8_t header[SC_PACKET_HEADER_SIZE];
    ssize_t r = socket.recv_all(header, SC_PACKET_HEADER_SIZE);
    if (r < SC_PACKET_HEADER_SIZE) {
        return false;
    }

    uint64_t pts_flags = sc_read64be(header);
    uint32_t len = sc_read32be(&header[8]);
    /*  printf("PTS flags: %" PRIx64 "\n", pts_flags);
      printf("Packet size: %" PRIx32 "\n", len);*/
    assert(len);

    if (av_new_packet(packet, len)) {
        LOG_OOM();
        return false;
    }

    r = socket.recv_all(packet->data, len);
    if (r < 0 || ((uint32_t)r) < len) {
        av_packet_unref(packet);
        return false;
    }

    if (pts_flags & SC_PACKET_FLAG_CONFIG) {
        packet->pts = AV_NOPTS_VALUE;
    }
    else {
        packet->pts = pts_flags & SC_PACKET_PTS_MASK;
    }

    if (pts_flags & SC_PACKET_FLAG_KEY_FRAME) {
        packet->flags |= AV_PKT_FLAG_KEY;
    }

    packet->dts = packet->pts;
    return true;
}

int ScDemuxer::run(void* data) {
       ScDemuxer* demuxer = static_cast<ScDemuxer*>(data);

        // Flag to report end-of-stream (i.e. device disconnected)
      //  enum sc_demuxer_status status = SC_DEMUXER_STATUS_ERROR;
        uint32_t raw_codec_id;
        bool ok = demuxer->recv_codec_id(&raw_codec_id);
        if (!ok) {
            LOGE("demuxer '%s': stream disabled due to connection error",
                demuxer->name);
           // // goto end;
        }

        if (raw_codec_id == 0) {
            LOGW("Demuxer '%s': stream explicitly disabled by the device",
                demuxer->name);
            sc_packet_source_sinks_disable(&demuxer->packet_source);
           /* status = SC_DEMUXER_STATUS_DISABLED;
            // goto end;*/
        }

        if (raw_codec_id == 1) {
            LOGE("Demuxer '%s': stream configuration error on the device",
                demuxer->name);
           // // goto end;
        }

        enum AVCodecID codec_id = demuxer->to_avcodec_id(raw_codec_id);
        printf("Codec id: %08x\n", raw_codec_id);
        if (codec_id == AV_CODEC_ID_NONE) {
            LOGE("Demuxer '%s': stream disabled due to unsupported codec",
                demuxer->name);
            sc_packet_source_sinks_disable(&demuxer->packet_source);
          //  // goto end;
        }

        const AVCodec* codec = avcodec_find_decoder(codec_id);
        if (!codec) {
            LOGE("Demuxer '%s': stream disabled due to missing decoder",
                demuxer->name);
            sc_packet_source_sinks_disable(&demuxer->packet_source);
           // // goto end;
        }

        AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
        if (!codec_ctx) {
            LOG_OOM();
           // // goto end;
        }
        codec_ctx->flags |= AV_CODEC_FLAG_LOW_DELAY;

        if (codec->type == AVMEDIA_TYPE_VIDEO) {
            uint32_t width;
            uint32_t height;
            ok = demuxer->recv_video_size(&width, &height);
            if (!ok) {
               // // goto finally_free_context;
            }

            codec_ctx->width = width;
            codec_ctx->height = height;
            printf("Width = %d\nHeight = %d\n", width, height);
            codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
        }
        else {
            // Hardcoded audio properties
   /* #ifdef SCRCPY_LAVU_HAS_CHLAYOUT
            codec_ctx->ch_layout = (AVChannelLayout) AV_CHANNEL_LAYOUT_STEREO;
    #else
            codec_ctx->channel_layout = AV_CH_LAYOUT_STEREO;
            codec_ctx->channels = 2;
    #endif*/
            codec_ctx->sample_rate = 48000;

            if (raw_codec_id == SC_CODEC_ID_FLAC) {
                // The sample_fmt is not set by the FLAC decoder
                codec_ctx->sample_fmt = AV_SAMPLE_FMT_S16;
            }
        }

        if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
            LOGE("Demuxer '%s': could not open codec", demuxer->name);
            // goto finally_free_context;
        }

        if (!sc_packet_source_sinks_open(&demuxer->packet_source, codec_ctx)) {
            LOGE("Unable to open packet source");
            // goto finally_free_context;
        }

        // Config packets must be merged with the next non-config packet only for
        // H.26x
        bool must_merge_config_packet = raw_codec_id == SC_CODEC_ID_H264
            || raw_codec_id == SC_CODEC_ID_H265;

        SCPacketMerger merger;

        if (must_merge_config_packet) {
            merger.init(); // Initialize the merger
        }

        AVPacket* packet = av_packet_alloc();
        if (!packet) {
            LOG_OOM();
            //// goto finally_close_sinks;
        }
        int i = 0;
        for (;;) {
            bool ok = demuxer->recv_packet(packet);
            if (!ok) {
                // end of stream
              //  status = SC_DEMUXER_STATUS_EOS;
                break;
            }
            if (i++ % 10 == 0)
                printf("Packet receiving\n");

            if (must_merge_config_packet) {
                // Prepend any config packet to the next media packet
                ok = merger.merge(packet); // Merge the packet using SCPacketMerger
                if (!ok) {
                    av_packet_unref(packet);
                    break;
                }
            }

           // ok = sc_packet_source_sinks_push(&demuxer->packet_source, packet);
            av_packet_unref(packet);
            if (!ok) {
                // The sink already logged its concrete error
                break;
            }
        }

        LOGD("Demuxer '%s': end of frames", demuxer->name);

        if (must_merge_config_packet) {
            merger.destroy(); // Destroy the merger
        }

        av_packet_free(&packet);
    finally_close_sinks:
       // sc_packet_source_sinks_close(&demuxer->packet_source);
    finally_free_context:
        // This also calls avcodec_close() internally
        avcodec_free_context(&codec_ctx);
    end:
        //demuxer->cbs->on_ended(demuxer, status, demuxer->cbs_userdata);

        return 0;
}

void ScDemuxer::init(const char* name, ScSocket socket) {
    // assert(socket != SC_SOCKET_NONE);

    this->name = name;
    this->socket = socket;
    sc_packet_source_init(&packet_source);
}

bool ScDemuxer::start() {
    LOGD("Demuxer '%s': starting thread", name);

    bool ok = sc_thread_create(&thread, run, "scrcpy-demuxer", this);
    if (!ok) {
        LOGE("Demuxer '%s': could not start thread", name);
        return false;
    }
    return true;
}

void ScDemuxer::join() {
    sc_thread_join(&thread, NULL);
}