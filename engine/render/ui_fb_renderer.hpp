#pragma once

#include "base_renderer.hpp"

namespace spk {
    class render_system_t;

    class ui_framebuffer_renderer_t : public base_framebuffer_renderer_t {
    public:
        void init() override;
        void free() override;
        void draw(render_system_t* rs, id_t current) override;

    private:
        program_t        program;
        vertex_layout_t  vertex_layout;
        vertex_array_t   vertex_array;
        vertex_buffer_t  vertex_buffer;
    };
}