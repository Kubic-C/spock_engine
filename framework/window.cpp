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
        window_t* window = (window_t*)glfwGetWindowUserPointer(window_);

        if(window->resize_callback.fp_callback)
            window->resize_callback.fp_callback(window, window->resize_callback.data, width, height);
    }

    void key_callback(GLFWwindow* window_, int key, int scancode, int action, int mods) {
        window_t* window = (window_t*)glfwGetWindowUserPointer(window_);

        if(window->char_callback.fp_callback) 
            window->char_callback.fp_callback(window, window->char_callback.data, key);        
    }

    void mouse_button_callback(GLFWwindow* window_, int button, int action, int mods) {
        window_t* window = (window_t*)glfwGetWindowUserPointer(window_);

        if(window->mouse_callback.fp_callback)
            window->mouse_callback.fp_callback(window, window->mouse_callback.data, button, action, mods); 

    }

    void error_callback(int error_code, const char* message) {
        log.log(sfk::LOG_TYPE_ERROR, "error code(%i), msg: %s", error_code, message);
    }

    window_t::window_t() {}

    window_t::~window_t() {}

    void window_t::init(int w, int h, std::string title) {
        glfwSetErrorCallback(error_callback);
        
        c_window = glfwCreateWindow(w, h, title.c_str(), null, null);
        sfk_assert(c_window);


        glfwSetFramebufferSizeCallback(c_window, _framebuffer_size_callback);
        glfwSetKeyCallback(c_window, key_callback);
        glfwSetWindowUserPointer(c_window, this);
        glfwSetInputMode(c_window, GLFW_STICKY_KEYS, GLFW_TRUE);
        glfwSetMouseButtonCallback(c_window, mouse_button_callback);
    }

    void window_t::free() {
        glfwDestroyWindow(c_window); 
    }
}