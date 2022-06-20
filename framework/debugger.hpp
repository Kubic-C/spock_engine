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

    class deubg_logger {
    public:
        struct log {
            std::string what;
            log_type type;
        };

        template<typename ... Args>
        void add_log(log_type type, Args&& ... _args) {
            std::stringstream sstream;

            sstream << "[" << type << ", SPOCK" << ", time(" << time(0) << ")]: ";
            ((sstream << _args), ...);

            std::cout << sstream.str() << std::endl;

            logs.push(log{sstream.str(), type});
        }

    private:
        std::queue<log> logs;
    };
}