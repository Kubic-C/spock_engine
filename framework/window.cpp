#include "window.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
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
        pd->nwh = glfwGetWin32Window(c_window);
        bgfx::setPlatformData(*pd);
    }
}