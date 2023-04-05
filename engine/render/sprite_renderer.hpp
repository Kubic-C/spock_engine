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
        struct vertex_t {
            glm::vec3 pos;
            glm::vec3 tex;
        };

        sprite_renderer_t(const flecs::world& world) {
            mesh_system_add(world.system<comp_rigid_body_t, comp_sprite_t>().iter(sprite_mesh));
            mesh_system_add(world.system<comp_rigid_body_t, comp_particles_t>().iter(particles_mesh));
        }

        static void particles_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_particles_t* particles);
        static void sprite_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_sprite_t* sprites);
        void render() override;

        std::map<uint32_t, mesh_t<vertex_t, 6, 4>> meshes; 

    private:
        void add_mesh(const sprite_arrayd_t& sprite, void* vertices);
    };
}