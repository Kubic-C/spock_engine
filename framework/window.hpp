#pragma once

#include "base.hpp"
#include "debugger.hpp"

namespace sfk {
    class window {
    public:
        window(int width, int height, std::string name);
        ~window();

        inline void set_name(std::string name) {
            glfwSetWindowTitle(c_window, name.c_str());
        }

        inline void set_size(int x, int y) {
            glfwSetWindowSize(c_window, x, y);
        }

        inline void set_position(int x, int y) {
            glfwSetWindowPos(c_window, x, y);
        }

        inline void set_as_current_context() {
            glfwMakeContextCurrent(c_window);
        }

    private:
        GLFWwindow* c_window;
    };  
}