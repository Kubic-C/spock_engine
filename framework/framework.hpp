#pragma once

#include "entity.hpp"
#include "opengl.hpp"
#include "window.hpp"
#include "memory.hpp"

namespace spk {
    struct transform_tt { // used by a large majority of components, so it would make some sense to put it here
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };
}

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
            logger.add_log(type, _args...);
        }

    private:
    };
}