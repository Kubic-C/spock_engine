#pragma once

// dependencies 
#include <flecs.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <box2d/box2d.h>
#include <ft2build.h>
#include FT_FREETYPE_H

// extra
#include <cassert>
#include <stdint.h>
#include <stdarg.h>
#include <chrono>
#include <functional>
#include <any>

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

#ifdef _WIN32 
#include <Windows.h>

#define  MAIN \
WINAPI int WinMain( \
_In_ HINSTANCE hInstance, \
_In_opt_ HINSTANCE hPrevInstance, \
_In_ LPSTR lpCmdLine, \
_In_ int nShowCmd \
)
#else // linux or mac
#define MAIN main(int argc, argv**)
#endif  // _WIN32

#ifndef NDEBUG
#define DEBUG_EXPR(expr) expr
#else 
#define DEBUG_EXPR(expr)
#endif 

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