#pragma once

#include "../systems.hpp"
#include "../components/sprite.hpp"
#include "../components/tilebody.hpp"
#include "render.hpp"

namespace spk {
    struct sprite_render_system_ctx_t {
        struct vertex_t {
            glm::vec2 pos;
            glm::vec2 tex_coords;
        };

        void init();
        void free();

        void draw_atlas_meshes();
        void add_sprite_mesh(b2Body* body, comp_sprite_t& sprite, glm::vec2 offset = {0.0f, 0.0f});

        sfk::vertex_layout_t vertex_layout;
        sfk::vertex_buffer_t vertex_buffer;
        sfk::vertex_array_t vertex_array;
        sfk::program_t program;

        static constexpr uint32_t vertexes_per_sprite = 6;
        static constexpr uint32_t indexes_per_sprite = 4;

        struct atlas_mesh_t {
            std::vector<vertex_t> mesh;
            uint32_t sprites;
        } meshes[SPK_MAX_ATLAS];
    };

    void sprite_render_cs_init(system_ctx_allocater_t& allocater, flecs::world& world);
}