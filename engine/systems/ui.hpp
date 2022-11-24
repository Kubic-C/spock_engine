#pragma once

#include "../systems.hpp"
#include "../components/ui.hpp"

namespace spk {
    struct ui_system_ctx_t {
        float t;
    };

    struct ui_render_system_ctx_t {
        struct vertex_t {
            glm::vec2 pos;
            glm::vec3 color;
        };

        glm::mat4 vp;

        sfk::vertex_array_t vertex_array;
        sfk::vertex_layout_t vertex_layout;
        sfk::vertex_buffer_t vertex_buffer;
        sfk::program_t program;
        
        std::vector<vertex_t> mesh;
        uint32_t vertices;
    };  

    void ui_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world);
}