#pragma once

#include "base.hpp"
#include "debugger.hpp"

namespace sfk {
    struct window_t;

    typedef void(*resize_callback_t)(window_t* window, void* data, int width, int height);
    typedef void(*char_callback_t)(window_t* window, void* data, uint32_t codepoint);
    typedef void(*mouse_button_callback_t)(window_t* window,void* data, int button, int action, int mods);

    template<typename T>
    struct window_callback_t {
        T fp_callback = null;
        void* data = null;
    };

    struct window_t {
        window_t();
        ~window_t();

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

        inline int get_key(int key) {
            return glfwGetKey(c_window, key);
        }

        inline bool closed() {
            return glfwWindowShouldClose(c_window);
        }

        inline void close() {
            glfwSetWindowShouldClose(c_window, true);
        }

        // gl related functions

        inline void swap_buffers() {
            glfwSwapBuffers(c_window);
        }

        inline void make_context_current() {
            glfwMakeContextCurrent(c_window);
        }

        inline void gl_version(int major, int minor) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }

        GLFWwindow* c_window;   
        window_callback_t<resize_callback_t> resize_callback;
        window_callback_t<char_callback_t> char_callback;
        window_callback_t<mouse_button_callback_t> mouse_callback;
    };  
}