#pragma once

#include "./window.hpp"
#include "../systems.hpp"

namespace spk {
    struct render_system_ctx_t {
        glm::mat4 vp;
    };

    void render_system_on_add(flecs::entity e, render_system_ctx_t& ctx);
    void render_system_on_remove(flecs::entity e, render_system_ctx_t& ctx);
    void render_system_pre_update(flecs::entity e, comp_window_t& window, tag_current_window_t);
    void render_system_resize(flecs::iter& iter);
    void render_system_post_update(flecs::entity e, comp_window_t& window, tag_current_window_t);
    void render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world); 
}