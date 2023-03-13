#pragma once 

#define SPOCK_ENGINE 1

// dependencies 
#include <box2d/box2d.h>
#include <flecs.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glad/gl.h>
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
#include <list>
#include <map>
#include <thread>
#include <array>
#include <memory>
#include <bitset>
#include <algorithm> 
#include <limits>

// engine
#include "logger.hpp"
#include "debug.hpp"
#include "pipeline.hpp"

#define FLOAT_MAX std::numeric_limits<float>::max()
#define FLOAT_MIN std::numeric_limits<float>::min() 

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

namespace spk {
    template<typename T>
    T* get_ctx(flecs::iter& iter) {
        return static_cast<T*>(iter.ctx());
    }

    template<typename T>
    T&   ctor(T* obj) {
        new(obj)T();

        return *obj;
    }

    template<typename T>
    void dtor(T* obj) {
        obj->~T();
    }

    // double are used for high defenition time keeping
    struct time_t {
        std::chrono::time_point<std::chrono::steady_clock> tp;

        time_t() {
            tp = std::chrono::steady_clock::now();
        }

        ~time_t() {

        }

        double get_time() {
            auto ep = std::chrono::steady_clock::now();

            std::chrono::duration<double> duration = ep - tp;

            return (duration).count();
        }
    };

    inline time_t time;
}

