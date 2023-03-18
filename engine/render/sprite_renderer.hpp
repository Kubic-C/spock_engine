#pragma once

#include "common.hpp"
#include "components/rigid_body.hpp"
#include "components/sprite.hpp"

namespace spk {
    struct sprite_renderer_t : base_renderer_t {
    public:
        sprite_renderer_t(flecs::world& world) {
            mesh_system_add<comp_rigid_body_t, comp_sprite_t>(world, sprite_mesh);
        }

        struct mesh_t {
            vertex_buffer_t buffer;
            size_t          vertexes_on_buffer;
            size_t          vertexes_to_render;
        };

        std::array<mesh_t, SPK_MAX_SPRITE_ARRAYS> meshes; 

        static void sprite_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_sprite_t* sprites);
        void render() override;
    };
}