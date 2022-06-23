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
    void _framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        bgfx::setViewRect(0, 0, 0, width, height);
        bgfx::reset(width, height);
        std::cout << "RESIZE\n";
    }

    window::window() {}

    window::~window() {}

    void window::init(int w, int h, std::string title) {
        c_window = glfwCreateWindow(w, h, title.c_str(), null, null);
        glfwSetFramebufferSizeCallback(c_window, _framebuffer_size_callback);
        assert(c_window);
    }

    void window::free() {
        glfwDestroyWindow(c_window); 
    }

    void window::set_platform_data(bgfx::PlatformData* pd) {
        pd->nwh = (void*)(uintptr_t)_getNativeWindow(c_window);
        pd->ndt = (void*)(uintptr_t)_getNativeDisplay(c_window);
        bgfx::setPlatformData(*pd);
    }
}