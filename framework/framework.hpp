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
    class sfk {
    public:
        void init();
        void free();

        inline float get_time() {
            return glfwGetTime();
        }

        template<typename ... args>
        void log(log_type type, args&& ... _args) {
            logger.add_log(type, _args...);
        }

    private:
    };

    constexpr float ppm = 8.0f;
}