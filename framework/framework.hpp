#pragma once

#include "entity.hpp"
#include "opengl.hpp"
#include "window.hpp"
#include "memory.hpp"

namespace spk {

    struct transform_t { // used by a large majority of components, so it would make some sense to put it here
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };
}

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