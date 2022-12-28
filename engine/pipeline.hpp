#pragma once

#include "base.hpp"

namespace spk {
    struct render_system_t   {};
    struct game_system_t     {};

    inline flecs::entity render_system = flecs::entity(UINT64_MAX);
    inline flecs::entity_t on_render_begin = flecs::PreUpdate;
    inline flecs::entity_t on_render       = flecs::OnUpdate;
    inline flecs::entity_t on_render_end   = flecs::PostUpdate;

    inline flecs::entity game_system = flecs::entity(UINT64_MAX);
    inline flecs::entity on_pre_update = flecs::entity(UINT64_MAX);
    inline flecs::entity on_update = flecs::entity(UINT64_MAX);
    inline flecs::entity on_post_update = flecs::entity(UINT64_MAX);

    inline void init_phases(flecs::world& world) {
        render_system = world.component<render_system_t>();

        /* https://discord.com/channels/633826290415435777/633826290415435781/1057124064025661560 */
        on_render_begin = world.entity().add(flecs::Phase); //.depends_on(flecs::OnUpdate);
        on_render       = world.entity().add(flecs::Phase).depends_on(on_render_begin);
        on_render_end   = world.entity().add(flecs::Phase).depends_on(on_render); 

        game_system = world.component<game_system_t>();
        on_pre_update = world.entity().add(flecs::Phase);
        on_update = world.entity().add(flecs::Phase).depends_on(on_pre_update);
        on_pre_update = world.entity().add(flecs::Phase).depends_on(on_update);
    }
}