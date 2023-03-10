#pragma once

#include "base_renderer.hpp"
#include "components/sprite.hpp"

namespace spk {
    struct sprite_atlasd_vertex_t {
        glm::vec3 pos;
        glm::vec2 tex_coords;
    };

    struct sprite_arrayd_vertex_t {
        glm::vec3 pos;
        glm::vec3 tex_coords;
    };

    template<typename Vt, typename St>
    struct sprite_batch_mesh_t : public mesh_t {
        void add_sprite_mesh(
            b2Body* body, 
            St& sprite,
            uint32_t sprites_x,
            uint32_t sprites_y,
            const glm::vec2& offset);
        
        void add_sprite_mesh(
            St& sprite, 
            const glm::vec2& _1, 
            const glm::vec2& _2, 
            const glm::vec2& _3, 
            const glm::vec2& _4);

        void add_sprite_mesh(b2Body* body, St& sprite, glm::vec2 offset = {0.0f, 0.0f});

        struct atlas_mesh_t {
            std::vector<Vt> mesh;
            uint32_t sprites;
        } meshes[SPK_MAX_ATLAS];

        static const uint32_t vertexes_per_sprite = 6;
        static const uint32_t indexes_per_sprite  = 4;

        void init();
        void free();

        void resize_mesh_if_need(size_t atlas);
        void subdata(uint32_t atlas);
    };

    typedef sprite_batch_mesh_t<sprite_atlasd_vertex_t, comp_sprite_atlasd_t> sprite_atlasd_batch_mesh_t;
    typedef sprite_batch_mesh_t<sprite_arrayd_vertex_t, comp_sprite_arrayd_t> sprite_arrayd_batch_mesh_t;

    class sprite_renderer_t : public base_renderer_t {
    public:

        sprite_renderer_t();
        ~sprite_renderer_t();
        void draw() override;

        sprite_atlasd_batch_mesh_t atlasd_sprites;
        sprite_arrayd_batch_mesh_t arrayd_sprites;

    private:
        render_ctx_t atlasd_ctx;
        render_ctx_t arrayd_ctx;
    };

}

namespace spk {
    template<>
    void sprite_batch_mesh_t<sprite_atlasd_vertex_t, comp_sprite_atlasd_t>::add_sprite_mesh(
            comp_sprite_atlasd_t& sprite, 
            const glm::vec2& _1, 
            const glm::vec2& _2, 
            const glm::vec2& _3, 
            const glm::vec2& _4);

    template<>
    void sprite_batch_mesh_t<sprite_arrayd_vertex_t, comp_sprite_arrayd_t>::add_sprite_mesh(
            comp_sprite_arrayd_t& sprite, 
            const glm::vec2& _1, 
            const glm::vec2& _2, 
            const glm::vec2& _3, 
            const glm::vec2& _4_);

    template<>
    void sprite_batch_mesh_t<sprite_arrayd_vertex_t, comp_sprite_arrayd_t>::add_sprite_mesh(
            b2Body* body, 
            comp_sprite_arrayd_t& sprite,
            uint32_t sprites_x,
            uint32_t sprites_y,
            const glm::vec2& offset);

    template<typename Vt, typename St>
    void sprite_batch_mesh_t<Vt, St>::init() {
        m_init();
        max_vertexes = 1024;

        vertex_buffer.buffer_data(sizeof(Vt) * max_vertexes, nullptr, GL_DYNAMIC_DRAW);

        for(auto& mesh : meshes)  {
            mesh.mesh.resize(max_vertexes);
        }       
    }

    template<typename Vt, typename St>
    void sprite_batch_mesh_t<Vt, St>::free() {
        m_free();
    }

    template<typename Vt, typename St>
    void sprite_batch_mesh_t<Vt, St>::add_sprite_mesh(b2Body* body, St& sprite, glm::vec2 offset) {
        add_sprite_mesh(sprite, 
            (glm::vec2)body->GetWorldPoint(b2Vec2(-sprite.size.x, -sprite.size.y) + offset),
            (glm::vec2)body->GetWorldPoint(b2Vec2( sprite.size.x, -sprite.size.y) + offset),
            (glm::vec2)body->GetWorldPoint(b2Vec2( sprite.size.x,  sprite.size.y) + offset),
            (glm::vec2)body->GetWorldPoint(b2Vec2(-sprite.size.x,  sprite.size.y) + offset));
    }

    template<typename Vt, typename St>
    void sprite_batch_mesh_t<Vt, St>::resize_mesh_if_need(size_t atlas) {
        size_t sprite_vertex_count = meshes[atlas].sprites * indexes_per_sprite;

        if(sprite_vertex_count > meshes[atlas].mesh.size()) {
            meshes[atlas].mesh.resize(sprite_vertex_count * 2);
        }
    }

    template<typename Vt, typename St>
    void sprite_batch_mesh_t<Vt, St>::subdata(uint32_t atlas) {
        size_t sprite_vertex_count = meshes[atlas].sprites * indexes_per_sprite;

        if(sprite_vertex_count > max_vertexes) {
            resize(sizeof(Vt), sprite_vertex_count * 2, 0);
            
            max_vertexes = sprite_vertex_count * 2;
        }

        vertex_buffer.buffer_sub_data(0, sizeof(Vt) * sprite_vertex_count, meshes[atlas].mesh.data());
    }
}
