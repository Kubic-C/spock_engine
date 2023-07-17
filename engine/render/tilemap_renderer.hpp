/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "render/common.hpp"
#include "components/tilemap.hpp"

namespace spk {
    struct comp_tilemap_t;
    struct tile_chunk_mesh_t;

    struct tilemap_renderer_t : public component_renderer_t {
    public:
        tilemap_renderer_t(const flecs::world& world) {
            mesh_system_add(world.system<comp_rigid_body_t, comp_tilemap_t>().iter(tilemap_mesh));

            // this'll be called after tilemap_mesh
            mesh_system_add(world.system<comp_rigid_body_t, comp_tilemap_t>().iter(tilemap_render));
        }

        static void tilemap_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_tilemap_t* tilemap);
        static void tilemap_render(flecs::iter& iter, comp_rigid_body_t* bodies, comp_tilemap_t* tilemap);
        void render() override {};

        void _chunk_render(kin::rigid_body_t* body, tile_chunk_mesh_t& mesh);
    };
}