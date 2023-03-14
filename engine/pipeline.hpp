#pragma once

#include "base.hpp"

namespace spk {
    struct tag_render_system_t   {};
    struct tag_renderer_t        {};
    struct tag_game_system_t     {};

    inline flecs::entity   render_system_id   = flecs::entity(UINT64_MAX);
    inline flecs::entity   renderer_id        = flecs::entity(UINT64_MAX);
    inline flecs::entity_t on_render_begin_id = UINT64_MAX;
    inline flecs::entity_t on_render_id       = UINT64_MAX;
    inline flecs::entity_t on_render_end_id   = UINT64_MAX;

    inline flecs::entity_t on_pre_mesh_id  = UINT64_MAX;
    inline flecs::entity_t on_mesh_id      = UINT64_MAX;
    inline flecs::entity_t on_post_mesh_id = UINT64_MAX;

    inline flecs::entity   game_system_id    = flecs::entity(UINT64_MAX);
    inline flecs::entity_t on_pre_update_id  = flecs::PreUpdate;
    inline flecs::entity_t on_update_id      = flecs::OnUpdate;
    inline flecs::entity_t on_post_update_id = flecs::PostUpdate; 

    inline void init_phases(flecs::world& world) {
        render_system_id = world.component<tag_render_system_t>();
        renderer_id      = world.component<tag_renderer_t>();

        on_render_begin_id = world.entity().add(flecs::Phase); //.depends_on(flecs::OnUpdate);
        on_render_id       = world.entity().add(flecs::Phase).depends_on(on_render_begin_id);
        on_render_end_id   = world.entity().add(flecs::Phase).depends_on(on_render_id); 

        on_pre_mesh_id  = world.entity().add(flecs::Phase).depends_on(flecs::PostUpdate);
        on_mesh_id      = world.entity().add(flecs::Phase).depends_on(on_pre_mesh_id);
        on_post_mesh_id = world.entity().add(flecs::Phase).depends_on(on_mesh_id);

        game_system_id = world.component<tag_game_system_t>();
    }
}