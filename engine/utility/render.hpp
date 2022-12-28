#pragma once

#include "opengl.hpp"

namespace spk {
    struct on_render_begin;
    struct on_render;
    struct on_render_end;

    class renderer_t {
        vertex_buffer_t vb;
        vertex_array_t  va;
        vertex_layout_t vl;
        program_t       program;
    };
}