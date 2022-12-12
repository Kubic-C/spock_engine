#include "debugger.hpp"

namespace sfk {
    info_logger_t::info_logger_t() {
        buf.resize(4096);
        log_file.open("./log.txt");
        
        flags |= LOG_FLAGS_ENABLE_STD_PIPE;

        log(LOG_TYPE_INFO, "info_logger_init");
    }

    info_logger_t::~info_logger_t() {
        log_file.close();
    }

    const char* type_to_str(log_type_e type) {
        switch(type) {
            case LOG_TYPE_ERROR:
                return "ERROR";

            case LOG_TYPE_ASSERT:
                return "ASSERTION FAILED";

            case LOG_TYPE_INFO:
                return "INFO";

            case LOG_TYPE_MISC:
                return "MSIC";

            default:
                return "UNKOWN";
        };
    }

    void info_logger_t::log(log_type_e type, const char* format, ...) {
        size_t ptr = 0;
        char* msg_begin = buf.data(); 
        char* msg_ptr = msg_begin;
        va_list args;
        
        ptr += sprintf(msg_ptr, "[SPOCK: %f] (%s) ", time.get_time(), type_to_str(type));
        msg_ptr = buf.data() + ptr;

        va_start(args, format);
        ptr += vsprintf(msg_ptr, format, args);
        msg_ptr = buf.data() + ptr;
        va_end(args);

        *msg_ptr = '\n';
        *(++msg_ptr) = '\0';
        ptr += 1; 

        if(flags & LOG_FLAGS_ENABLE_STD_PIPE) {
            FILE* pipe = nullptr;

            switch(type) {
            case LOG_TYPE_ASSERT: case LOG_TYPE_ERROR:
                pipe = stderr;
                break;

            default:
                pipe = stdout;
                break;
            };

            fwrite(msg_begin, sizeof(char), strlen(msg_begin) + 1, pipe);
        }

        if(type == LOG_TYPE_ERROR) {
            log(LOG_TYPE_INFO, "full unsafe exit due to error");
            exit(EXIT_FAILURE);
        }

        log_file.write(buf.data(), ptr);
    }

    void info_logger_t::spew() {
        printf("%s", buf.data());
    }

    void _assert(const char* file, const char* func, int line, const char* expr) {
        log.log(LOG_TYPE_ASSERT, "file(%s), func(%s), line(%i), expr(%s)\n", file, func, line, expr);
        abort();
    }
}