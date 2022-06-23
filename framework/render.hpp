#pragma once

#include "base.hpp"
#include "debugger.hpp"
#include <bx/math.h>

// utility render shit

namespace sfk {
    bgfx::ShaderHandle load_shader(debug_logger& logger, const std::string& file_name);
    bgfx::ShaderHandle load_custom_shader(debug_logger& logger, const std::string& file_path);

    class matrix4x4 {
        inline matrix4x4() {
            identity();
        }

        inline void identity() {
            bx::mtxIdentity(mtx);
        }


        float mtx[16];
    };
}