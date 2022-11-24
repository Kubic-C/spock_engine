#pragma once

#include "./window.hpp"
#include "../systems.hpp"

namespace spk {
    struct render_system_ctx_t {
        glm::mat4 vp;
        sfk::static_index_buffer_t quad_index_buffer;
    };

    void render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world); 
}