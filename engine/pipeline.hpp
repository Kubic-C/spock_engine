/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

    refer to license: https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "base.hpp"

namespace spk {
    struct tag_mesh_t          {};
    struct tag_renderer_t      {};
    struct tag_game_system_t   {};

    inline flecs::entity   renderer_id        = flecs::entity(UINT64_MAX);
    inline flecs::entity_t on_render_begin_id = UINT64_MAX;
    inline flecs::entity_t on_render_id       = UINT64_MAX;
    inline flecs::entity_t on_render_end_id   = UINT64_MAX;

    inline flecs::entity   game_system_id    = flecs::entity(UINT64_MAX);
    inline flecs::entity_t on_pre_update_id  = flecs::PreUpdate;
    inline flecs::entity_t on_update_id      = flecs::OnUpdate;
    inline flecs::entity_t on_post_update_id = flecs::PostUpdate; 

    inline void init_phases(const flecs::world& world) {
        renderer_id      = world.component<tag_renderer_t>();
        game_system_id   = world.component<tag_game_system_t>();

        on_render_begin_id = world.entity().add(flecs::Phase).depends_on(flecs::OnUpdate);
        on_render_id       = world.entity().add(flecs::Phase).depends_on(on_render_begin_id);
        on_render_end_id   = world.entity().add(flecs::Phase).depends_on(on_render_id); 

    }
}