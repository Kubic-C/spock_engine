#pragma once

#include "base.hpp"
#include "debugger.hpp"
#include <bx/math.h>

#define BGFX_HANDLE_VALID(handle) (handle.idx != bgfx::kInvalidHandle)

// utility render shit

namespace sfk {
    bgfx::ShaderHandle load_shader(const std::string& file_path);

    bgfx::ProgramHandle load_program(const std::string& vs, const std::string& fs);

    bool load_texture2D(const std::string& file_path, bgfx::TextureHandle& hdl);
}