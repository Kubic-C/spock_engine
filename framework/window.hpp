#pragma once

#include "base.hpp"
#include "debugger.hpp"

namespace sfk {
    struct window {
        window();
        ~window();

        void init(int w, int h, std::string title);
        void free();

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

        inline void get_size(int* x, int* y) {
            glfwGetWindowSize(c_window, x, y);
        }

        inline bool closed() {
            return glfwWindowShouldClose(c_window);
        }

        void set_platform_data(bgfx::PlatformData* pd);

        /* window buffer related handling */

        void set_view_clear();
        void set_view_rect();

        GLFWwindow* c_window;
    };  
}