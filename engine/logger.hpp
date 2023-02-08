#pragma once

#include "base.hpp"

#ifdef NDEBUG
#define spk_assert(expr, ...)
#define spk_trace()
#else 

#define SFK_FIRST_W_COMMA(arg, ...) #arg

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define spk_trace() \
    ::spk::trace_t _spk_trace(__FILENAME__, __PRETTY_FUNCTION__, __LINE__)

#define spk_assert(expr, ...) ((bool)(expr) ? void(0) : \
    ::spk::_assert(__FILE__, __PRETTY_FUNCTION__, __LINE__, #expr, SFK_FIRST_W_COMMA(__VA_ARGS__)))

#endif 

#define SFK_ANSI_RESET  "\u001B[0m"
#define SFK_ANSI_EMPHASIS  "\u001B[1m"
#define SFK_ANSI_ITALIC "\u001B[3m"
#define SFK_ANSI_BLACK  "\u001B[30m"
#define SFK_ANSI_RED "\u001B[31m"
#define SFK_ANSI_GREEN  "\u001B[32m"
#define SFK_ANSI_YELLOW  "\u001B[33m"
#define SFK_ANSI_BLUE  "\u001B[34m"
#define SFK_ANSI_PURPLE  "\u001B[35m"
#define SFK_ANSI_CYAN  "\u001B[36m"
#define SFK_ANSI_WHITE  "\u001B[37m"

// parsing ruling
// ruling is how the text should be displated
#define SFK_RULING_DEFAULT_TEXT_START '['
#define SFK_RULING_DEFAULT_EPHASIS_TEXT "emt"
#define SFK_RULING_DEFAULT_EMPHASIS   "em"
#define SFK_RULING_DEFAULT_ITALIC     "it"
#define SFK_RULING_DEFAULT_BLACK      "black"
#define SFK_RULING_DEFAULT_RED        "red"
#define SFK_RULING_DEFAULT_GREEN      "green"
#define SFK_RULING_DEFAULT_YELLOW     "yellow"
#define SFK_RULING_DEFAULT_BLUE       "blue"
#define SFK_RULING_DEFAULT_PURPLE     "purple"
#define SFK_RULING_DEFAULT_CYAN       "cyan"
#define SFK_RULING_DEFAULT_WHITE      "white"
#define SFK_RULING_DEFAULT_RESET      "reset"
#define SFK_RULING_DEFAULT_SEPERATOR  ','
#define SFK_RULING_DEFAULT_TEXT_END        ']'

// ruling results that arent ansi codes
#define SFK_OUTPUT_EMPHASIS_TEXT "[em] #===# [reset]" 

// ruling in logging:
// log.log("[em, red] my text is red and emphasized [reset]")

namespace spk {
    typedef std::map<std::string, const char*> rule_map_t;

    enum log_type_e {
        LOG_TYPE_ERROR,
        LOG_TYPE_ASSERT,
        LOG_TYPE_INFO,
        LOG_TYPE_MISC,
        LOG_TYPE_TRACE
    };

    enum log_flags_e: uint8_t {
        LOG_FLAGS_ENABLE_STD_PIPE = 1
    };

    struct info_logger_t {
        info_logger_t();
        ~info_logger_t();

        template<typename ... params>
        void log(const char* format, const params& ... args) {
            log(spk::LOG_TYPE_INFO, format, args...);
        }

        void log(log_type_e type, const char* format, ...);

        void spew(); // print the current buffer onto console
        bool trace(const char* file, const char* func, int line);

        rule_map_t rule_map;
        std::ofstream log_file;
        std::vector<char> buf;
        std::vector<std::string> trace_list;
        uint8_t flags;
    };

    class trace_t {
    public:
        trace_t() = delete;
        trace_t(const char* file, const char* func, int line);
        ~trace_t();
    
    private:
        bool error = false;
    };

    inline info_logger_t log;

    std::string parse_format(rule_map_t& map, const std::string& format);
    void _assert(const char* file, const char* func, int line, const char* expr, const char* msg);
    
    inline std::string strprintf(const char* format, ...) {
        va_list va_list;
        char buf[256];
        uint32_t n;

        va_start(va_list, format);
        n = vsprintf(buf, format, va_list);
        va_end(va_list);

        return std::string(buf, n);
    }
}