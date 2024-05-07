
#define SCRCPY_OPTIONS_H

#include "common.hpp"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "util/ticks.h"

enum sc_log_level {
    SC_LOG_LEVEL_VERBOSE,
    SC_LOG_LEVEL_DEBUG,
    SC_LOG_LEVEL_INFO,
    SC_LOG_LEVEL_WARN,
    SC_LOG_LEVEL_ERROR,
};
