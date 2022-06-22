#pragma once

#include "base.hpp"

#ifdef NDEBUG
#define DEBUG_ONLY(expr) 
#else 
#define DEBUG_ONLY(expr) expr
#endif 

namespace sfk {
    enum log_type {
        LOG_TYPE_INFO,
        LOG_TYPE_WARNING,
        LOG_TYPE_ERROR
    };

    class debug_logger {
    public:
        struct log {
            std::string what;
            log_type type;
        };

        template<typename ... params>
        void add_log(log_type type, const params& ... args) {
            std::stringstream sstream;

            sstream << "[" << type << ", SPOCK" << ", time(" << time(0) << ")]: ";
            ((sstream << args), ...);

            std::cout << sstream.str() << std::endl;

            logs.push(log{sstream.str(), type});
        }

    private:
        std::queue<log> logs;
    };
}