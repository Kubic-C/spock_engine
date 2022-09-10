#pragma once

// dependencies 
#include <flecs.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <box2d/box2d.h>
#include <ft2build.h>
#include FT_FREETYPE_H

// extra
#include <cassert>
#include <stdint.h>
#include <stdarg.h>
#include <chrono>

// streams
#include <iostream>
#include <fstream>
#include <sstream>

// containers
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <thread>
#include <array>
#include <memory>
#include <bitset>

// algorithms
#include <algorithm> 

namespace sfk {
    inline glm::vec2 to_glm_vec2(b2Vec2 vec2) { return (glm::vec2){vec2.x, vec2.y}; }
    inline b2Vec2 to_box_vec2(glm::vec2 vec2) { return (b2Vec2){vec2.x, vec2.y}; }

    struct time_t {
        std::chrono::time_point<std::chrono::steady_clock> tp;

        time_t() {
            tp = std::chrono::steady_clock::now();
        }

        ~time_t() {

        }

        float get_time() {
            auto ep = std::chrono::steady_clock::now();

            std::chrono::duration<float> duration = ep - tp;

            return (duration).count();
        }
    };

    inline time_t time;
}

#define null NULL