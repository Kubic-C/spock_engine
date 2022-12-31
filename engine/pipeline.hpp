#pragma once

#include "base.hpp"

namespace spk {
    struct tag_render_system_t   {};
    struct tag_renderer_t        {};
    struct tag_game_system_t     {};

    inline flecs::entity   render_system   = flecs::entity(UINT64_MAX);
    inline flecs::entity   renderer        = flecs::entity(UINT64_MAX);
    inline flecs::entity_t on_render_begin = UINT64_MAX;
    inline flecs::entity_t on_render       = UINT64_MAX;
    inline flecs::entity_t on_render_end   = UINT64_MAX;

    inline flecs::entity_t on_pre_mesh  = UINT64_MAX;
    inline flecs::entity_t on_mesh      = UINT64_MAX;
    inline flecs::entity_t on_post_mesh = UINT64_MAX;

    inline flecs::entity game_system = flecs::entity(UINT64_MAX);
    inline flecs::entity_t on_pre_update  = flecs::PreUpdate;
    inline flecs::entity_t on_update      = flecs::OnUpdate;
    inline flecs::entity_t on_post_update = flecs::PostUpdate; 

    inline void init_phases(flecs::world& world) {
        render_system = world.component<tag_render_system_t>();
        renderer      = world.component<tag_renderer_t>();

        on_render_begin = world.entity().add(flecs::Phase); //.depends_on(flecs::OnUpdate);
        on_render       = world.entity().add(flecs::Phase).depends_on(on_render_begin);
        on_render_end   = world.entity().add(flecs::Phase).depends_on(on_render); 

        on_pre_mesh  = world.entity().add(flecs::Phase).depends_on(flecs::PostUpdate);
        on_mesh      = world.entity().add(flecs::Phase).depends_on(on_pre_mesh);
        on_post_mesh = world.entity().add(flecs::Phase).depends_on(on_mesh);

        game_system = world.component<tag_game_system_t>();
    }
}