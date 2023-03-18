#pragma once

#include "render/common.hpp"
#include "components/rigid_body.hpp"
#include "components/tilemap.hpp"
#include "components/sprite.hpp"

namespace spk {
    struct sprite_renderer_t : base_renderer_t {
    public:
        sprite_renderer_t(flecs::world& world) {
            mesh_system_add(world.system<comp_rigid_body_t, comp_sprite_t>().iter(sprite_mesh));
            mesh_system_add(world.system<comp_rigid_body_t, comp_tilemap_t>().iter(tilemap_mesh));

            for(auto& mesh : meshes) {
                mesh.buffer.init(GL_ARRAY_BUFFER);
            }
        }

        struct mesh_t {
            vertex_buffer_t buffer;
            size_t          vertexes_on_buffer = 0;
            size_t          vertexes_to_render = 0;
        };

        std::array<mesh_t, SPK_MAX_SPRITE_ARRAYS> meshes; 

        void add_mesh(const sprite_arrayd_t& sprite, void* vertices);

        static void tilemap_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_tilemap_t* tilemap);
        static void sprite_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_sprite_t* sprites);
        void render() override;
    };
}