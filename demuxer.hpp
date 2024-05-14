#ifndef ScDemuxer_HPP
#define ScDemuxer_HPP

#include <cstdbool>
#include <cstdint>
#include <cassert>
#include "util/net.hpp"
#include "common.hpp"
#include "packet_merger.hpp"

#include "trait/packet_source.hpp"
#include "trait/packet_sink.hpp"
#include "util/thread.h"
#include <windows.h> 

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/time.h>
#include "events.h"
#include "util/binary.h"
#include "util/log.h"

}



#define SC_PACKET_HEADER_SIZE 12
#define SC_PACKET_FLAG_CONFIG    (UINT64_C(1) << 63)
#define SC_PACKET_FLAG_KEY_FRAME (UINT64_C(1) << 62)
#define SC_PACKET_PTS_MASK (SC_PACKET_FLAG_KEY_FRAME - 1)

enum class ScDemuxer_STATUS {
    EOS,
    DISABLED,
};



class ScDemuxer {
public:
    ScDemuxer();
    ~ScDemuxer() {}

    void init(const char* name);
    bool start();
    void join();
    ScSocket socket;
    struct sc_packet_source packet_source;


private:
    const char* name;
    sc_thread thread;

    static enum AVCodecID to_avcodec_id(uint32_t codec_id);
    bool recv_codec_id(uint32_t* codec_id);
    bool recv_video_size(uint32_t* width, uint32_t* height);
    bool recv_packet(AVPacket* packet);
    static int run(void* data);
};

#endif