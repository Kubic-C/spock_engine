#pragma once

#include "framework.hpp"

namespace spk {
    typedef void(*mouse_callback)(int button, int action, int mods);

    class input_tt {
    public:
        void init(GLFWwindow* window);
        void free();

        void mouse_callback(GLFWwindow* window, int button, int action, int mods);

    private:
    };
}