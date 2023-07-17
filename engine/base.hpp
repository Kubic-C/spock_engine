/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

    refer to license: https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once 

#define SPOCK_ENGINE 1

// dependencies 
#include <flecs.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <kin2d/kin2d.hpp>

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
#include <variant>

// engine
#include "logger.hpp"
#include "debug.hpp"
#include "pipeline.hpp"

#define FLOAT_MAX std::numeric_limits<float>::max()
#define FLOAT_MIN std::numeric_limits<float>::min() 

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

