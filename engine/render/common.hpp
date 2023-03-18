#pragma once

// common utilities for rendering

#include "opengl.hpp"
#include "utility/structures.hpp"
#include "core/data.hpp"

namespace spk {
    struct base_renderer_t {
        std::vector<flecs::system> mesh_systems;
        virtual void render() = 0;

        // framebuffer level = 0

        template<typename ... params, typename func>
        void mesh_system_add(flecs::world& world, func&& iter) {
            // adding the tag_render_system, stops it from being called during ticks
            mesh_systems.push_back(world.system<params...>().iter(iter));
            flecs::entity(world, mesh_systems.back().id()).add<tag_render_system_t>();
        }
    };
}