#pragma once

#include "base.hpp"
#include "debugger.hpp"

namespace sfk {
    bgfx::ShaderHandle load_shader(debug_logger& logger, const std::string& file_name);
    bgfx::ShaderHandle load_custom_shader(debug_logger& logger, const std::string& file_path);
}