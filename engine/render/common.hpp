#pragma once

// common utilities for rendering

#include "opengl.hpp"
#include "utility/structures.hpp"

namespace spk {
    struct base_renderer_t {
        std::vector<flecs::system> mesh_systems;
        virtual void render() = 0;

        // framebuffer level = 0

        flecs::system mesh_system_add(const flecs::system& system) {
            // adding the tag_render_system, stops it from being called during ticks
            mesh_systems.push_back(system);
            ((flecs::entity)system).add<tag_render_system_t>();

            return mesh_systems.back();
        }
    };
}