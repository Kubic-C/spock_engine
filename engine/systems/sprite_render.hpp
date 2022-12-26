#pragma once

#include "render.hpp"
#include "components/sprite.hpp"
#include "systems/particles.hpp"
#include "systems/tilebody.hpp"

/* the sprite render system is responsible for more then just rendering comp_sprite_t
    it also renders tile_bodies and particle_systems*/

namespace spk {
    struct sprite_render_system_ctx_t {
        struct vertex_t {
            glm::vec2 pos;
            glm::vec2 tex_coords;
        };

        void init();
        void free();

        void draw_atlas_meshes();
        void add_sprite_mesh( comp_sprite_t& sprite, const glm::vec2& _1, const glm::vec2& _2, const glm::vec2& _3, const glm::vec2& _4);

        void add_sprite_mesh(b2Body* body, comp_sprite_t& sprite, glm::vec2 offset = {0.0f, 0.0f});

        vertex_layout_t vertex_layout;
        vertex_buffer_t vertex_buffer;
        vertex_array_t vertex_array;
        program_t program;

        static constexpr uint32_t vertexes_per_sprite = 6;
        static constexpr uint32_t indexes_per_sprite = 4;

        struct atlas_mesh_t {
            std::vector<vertex_t> mesh;
            uint32_t sprites;
        } meshes[SPK_MAX_ATLAS];
    };

    void sprite_render_cs_init(system_ctx_allocater_t& allocater, flecs::world& world);
}