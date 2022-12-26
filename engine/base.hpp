#pragma once 

#define SPOCK_ENGINE 1

// dependencies 
#include <flecs.h>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glad/gl.h>
#include <box2d/box2d.h>
#include <ft2build.h>
#include FT_FREETYPE_H

// STL
#include <cassert>
#include <stdint.h>
#include <stdarg.h>
#include <chrono>
#include <functional>
#include <any>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <thread>
#include <array>
#include <memory>
#include <bitset>
#include <algorithm> 

// engine
#include "logger.hpp"
#include "debug.hpp"

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
#define MAIN int main(int argc, char** argv)
#endif  // _WIN32

#ifndef NDEBUG
#define SPK_DEBUG_EXPR(expr) expr
#define SPK_DEBUG_VALUE(type, name) type name
#else 
#define SPK_DEBUG_EXPR(expr)
#define SPK_DEBUG_VALUE(type, name)
#endif

#define null NULL

#define spk_register_component(world, type)\
        world.component<type>() \
            .on_add([](type& comp) { \
                SPK_DEBUG_EXPR_IF(log.log("on add hook | " #type), spk::DEBUG_FLAGS_ENABLE_HOOK); \
                comp.init(); \
            }) \
            .on_set([](type& comp) { \
                SPK_DEBUG_EXPR_IF(log.log("on set hook | " #type), spk::DEBUG_FLAGS_ENABLE_HOOK); \
            }) \
            .on_remove([](type& comp){  \
                SPK_DEBUG_EXPR_IF(log.log("on remove hook | " #type), spk::DEBUG_FLAGS_ENABLE_HOOK); \
                comp.free(); \
            }) 

namespace spk {
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

