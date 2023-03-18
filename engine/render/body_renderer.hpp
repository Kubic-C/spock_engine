#pragma once

#include "common.hpp"
#include "components/rigid_body.hpp"

namespace spk {
    struct body_renderer_t : base_renderer_t {
        body_renderer_t(flecs::world& world) {
            mesh_system_add<comp_rigid_body_t>(world, mesh);
        }

        size_t vertex_count;

        // renders all rigid bodies, used for debug
        static void mesh(flecs::iter& iter, comp_rigid_body_t* bodies);
        void render() override;
    };
}
