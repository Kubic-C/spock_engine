#include "framework.hpp"
#include <iostream>

namespace sfk {
    void print_name() {
        std::cout << "Sawyer Porter\n";
    }

    void sfk_t::init() {
        glfwInit();
    }

    void sfk_t::free() {
        glfwTerminate();
    }
}