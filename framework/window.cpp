#include "window.hpp"

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11
#define _getNativeWindow(c_window) glfwGetX11Window(c_window)
#define _getNativeDisplay(c_window) glfwGetX11Display()
#elif _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define _getNativeWindow(c_window) glfwGetWin32Window(c_window)
#endif // X11

#include "GLFW/glfw3native.h"


namespace sfk {
    void _framebuffer_size_callback(GLFWwindow* window_, int width, int height) {
        window_tt* window = (window_tt*)glfwGetWindowUserPointer(window_);

        if(window->resize_callback.fp_callback)
            window->resize_callback.fp_callback(window, window->resize_callback.data, width, height);
    }

    void key_callback(GLFWwindow* window_, int key, int scancode, int action, int mods) {
        window_tt* window = (window_tt*)glfwGetWindowUserPointer(window_);

        if(window->char_callback.fp_callback) 
            window->char_callback.fp_callback(window, window->char_callback.data, key);        
    }

    void mouse_button_callback(GLFWwindow* window_, int button, int action, int mods) {
        window_tt* window = (window_tt*)glfwGetWindowUserPointer(window_);

        if(window->mouse_callback.fp_callback)
            window->mouse_callback.fp_callback(window, window->mouse_callback.data, button, action, mods); 

    }

    window_tt::window_tt() {}

    window_tt::~window_tt() {}

    void window_tt::init(int w, int h, std::string title) {
        c_window = glfwCreateWindow(w, h, title.c_str(), null, null);
        assert(c_window);
        glfwSetFramebufferSizeCallback(c_window, _framebuffer_size_callback);
        glfwSetKeyCallback(c_window, key_callback);
        glfwSetWindowUserPointer(c_window, this);
        glfwSetInputMode(c_window, GLFW_STICKY_KEYS, GLFW_TRUE);
        glfwSetMouseButtonCallback(c_window, mouse_button_callback);
    }

    void window_tt::free() {
        glfwDestroyWindow(c_window); 
    }
}