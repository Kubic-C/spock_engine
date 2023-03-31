/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "render/common.hpp"
#include "components/rigid_body.hpp"

namespace spk {
    struct tag_body_render_t {

    };

    struct body_renderer_t : component_renderer_t {
        body_renderer_t(flecs::world& world) {
            mesh_system_add(
                world.system<comp_rigid_body_t>().term<tag_body_render_t>().iter(mesh));
        }

        size_t vertex_count = 0;

        // renders all rigid bodies, used for debug
        static void mesh(flecs::iter& iter, comp_rigid_body_t* bodies);
        void render() override;
    };
}
