#pragma once

#include "base.hpp"

namespace spk {
    enum debug_flags_e: uint64_t {
        DEBUG_FLAGS_ENABLE_HOOK = 1 << 0,
        DEBUG_FLAGS_ENABLE_STATE_CHANGE = 1 << 1,
        DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME = 1 << 2 // enable logging when engine_t::init run free, are called
    };

    struct debug_t {
        uint64_t flags = 0;
    };

#ifndef NDEBUG
    extern debug_t debug;
    #define SPK_DEBUG_EXPR_IF(expr, flag) \
        if(debug.flags & flag) { (expr); }
    #define SPK_DEBUG_LOG_IF(flag, ...) \
        SPK_DEBUG_EXPR_IF(::spk::log.log(__VA_ARGS__), flag)
    #define SPK_DEBUG_ENABLE(flag) (::spk::debug.flags |= flag)
    #define SPK_DEBUG_DISABLE(flag) (::spk::debug.flags &= ~flag)
#else // NOT DEBUG (release)
    #define SPK_DEBUG_EXPR_IF(expr, flag)
    #define SPK_DEBUG_LOG_IF(flag, format, ...)
    #define SPK_DEBUG_ENABLE(flag)
    #define SPK_DEBUG_DISABLE(flag)
#endif 
}