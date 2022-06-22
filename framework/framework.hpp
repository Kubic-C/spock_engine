#pragma once

#include "entity.hpp"
#include "render.hpp"
#include "window.hpp"

// spock framework //
namespace sfk {
    template<typename T>
    void zero(T* dst) {
        memset(dst, 0, sizeof(T));
    }

    template<typename T>
    T& make_null_reference() {
        return *(T*)nullptr;
    }

    void print_name();

    /* handling all external and internal library state */
    class sfk {
    public:
        void init();
        void free();

        inline float get_time() {
            return glfwGetTime();
        }

        template<typename ... args>
        void log(log_type type, args&& ... _args) {
            logger.add_log(type, _args);
        }

        debug_logger& get_logger() {
            return logger;
        }

    private:
        debug_logger logger;
    };
}