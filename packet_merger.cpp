#include "packet_merger.hpp"
#include "util/log.h"
#include <cstdlib>
#include <cstring>

SCPacketMerger::SCPacketMerger() : config(nullptr), config_size(0) {}

SCPacketMerger::~SCPacketMerger() {
    destroy();
}

void SCPacketMerger::init() {
    config = nullptr;
}

void SCPacketMerger::destroy() {
    free(config);
    config = nullptr;
}

bool SCPacketMerger::merge(AVPacket* packet) {
    bool is_config = packet->pts == AV_NOPTS_VALUE;

    if (is_config) {
        free(config);

        config = static_cast<uint8_t*>(malloc(packet->size));
        if (!config) {
            LOG_OOM();
            return false;
        }

        memcpy(config, packet->data, packet->size);
        config_size = packet->size;
    }
    else if (config) {
        size_t config_size = this->config_size;
        size_t media_size = packet->size;

        if (av_grow_packet(packet, static_cast<int>(config_size))) {
            LOG_OOM();
            return false;
        }

        memmove(packet->data + config_size, packet->data, media_size);
        memcpy(packet->data, config, config_size);

        free(config);
        config = nullptr;
    }

    return true;
}
