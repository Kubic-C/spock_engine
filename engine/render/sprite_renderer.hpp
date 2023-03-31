/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "render/common.hpp"
#include "components/rigid_body.hpp"
#include "components/tilemap.hpp"
#include "components/sprite.hpp"
#include "components/particles.hpp"

namespace spk {
    struct sprite_renderer_t : component_renderer_t {
    public:
        sprite_renderer_t(flecs::world& world) {
            mesh_system_add(world.system<comp_rigid_body_t, comp_sprite_t>().iter(sprite_mesh));
            mesh_system_add(world.system<comp_rigid_body_t, comp_tilemap_t>().iter(tilemap_mesh));
            mesh_system_add(world.system<comp_rigid_body_t, comp_particles_t>().iter(particles_mesh));
        }

        struct mesh_t {
            vertex_buffer_t buffer;
            size_t          vertexes_on_buffer = 0;
            size_t          vertexes_to_render = 0;

            mesh_t();
            ~mesh_t();
        };

        std::map<uint32_t, mesh_t> meshes; 

        static void particles_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_particles_t* particles);
        static void tilemap_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_tilemap_t* tilemap);
        static void sprite_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_sprite_t* sprites);
        void render() override;

    private:
        void add_mesh(const sprite_arrayd_t& sprite, void* vertices);
    };
}