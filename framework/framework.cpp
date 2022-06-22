#include "framework.hpp"
#include <iostream>

namespace sfk {
    void print_name() {
        std::cout << "Sawyer Porter\n";
    }

    void sfk::init() {
        glfwInit();
    }

    void sfk::free() {
        glfwTerminate();
    }
}