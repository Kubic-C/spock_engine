#include "window.hpp"

namespace sfk {

    window::window(int width, int height, std::string name) {
        c_window = glfwCreateWindow(width, height, name.c_str(), null, null);
        if(!c_window) {
            
        }
    }

    window::~window() {

    }
}