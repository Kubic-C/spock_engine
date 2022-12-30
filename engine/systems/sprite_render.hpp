#pragma once

#include "render.hpp"
#include "components/sprite.hpp"
#include "systems/particles.hpp"
#include "systems/tilebody.hpp"

/* the sprite render system is responsible for more then just rendering comp_sprite_t
    it also renders tile_bodies and particle_systems*/

namespace spk {
    struct sprite_vertex_t {
        glm::vec2 pos;
        glm::vec2 tex_coords;
    };

    struct sprite_batch_mesh_t : public comp_mesh_t {
        void add_sprite_mesh( comp_sprite_t& sprite, const glm::vec2& _1, const glm::vec2& _2, const glm::vec2& _3, const glm::vec2& _4);
        void add_sprite_mesh(b2Body* body, comp_sprite_t& sprite, glm::vec2 offset = {0.0f, 0.0f});

        struct atlas_mesh_t {
            std::vector<sprite_vertex_t> mesh;
            uint32_t sprites;
        } meshes[SPK_MAX_ATLAS];

        static const uint32_t vertexes_per_sprite = 6;
        static const uint32_t indexes_per_sprite  = 4;

        void init() { m_init(); }
        void free() { m_free(); }

        void gpu_write(uint32_t atlas);
    };

    class sprite_renderer_t : public base_renderer_t {
    public:

        void init() override;
        void free() override;
        void draw() override;

        // sprite rendering uses batch rendering
        sprite_batch_mesh_t sprites;
    };

    void sprite_render_cs_init(system_ctx_allocater_t& allocater, flecs::world& world);
}