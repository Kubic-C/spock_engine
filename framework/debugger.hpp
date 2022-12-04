#pragma once

#include "base.hpp"

#ifdef NDEBUG
#define DEBUG_ONLY(expr) 
#define sfk_assert(expr)
#else 
#define sfk_assert(expr) ((bool)(expr) ? void(0) : \
    sfk::_assert(__FILE__, __PRETTY_FUNCTION__, __LINE__, #expr))
#define DEBUG_ONLY(expr) expr
#endif 

namespace sfk {
    enum log_type_e {
        LOG_TYPE_ERROR,
        LOG_TYPE_ASSERT,
        LOG_TYPE_INFO,
        LOG_TYPE_MISC
    };

    enum log_flags_e: uint8_t {
        LOG_FLAGS_ENABLE_STD_PIPE = 1
    };

    struct info_logger_t {
        info_logger_t();
        ~info_logger_t();

        void log(const char* format, ...);
        void log(log_type_e type, const char* format, ...);
        void spew(); // print the current buffer onto console

        std::vector<char> buf;
        uint8_t flags;
        size_t ptr;
    };

    inline info_logger_t log;

    void _assert(const char* file, const char* func, int line, const char* expr);
}