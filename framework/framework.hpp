#pragma once

#include "entity.hpp"
#include "opengl.hpp"
#include "window.hpp"
#include "memory.hpp"

// spock framework //
namespace sfk {
    void print_name();

    /* handling all external and internal library state */
    class sfk_t {
    public:
        void init();
        void free();

        inline float get_time() {
            return time.get_time();
        }

    private:
    };

    constexpr float ppm = 8.0f;
}