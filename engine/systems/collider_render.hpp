#pragma once

#include "../systems.hpp"
#include "../components/box2D.hpp"
#include "render.hpp"

namespace spk {
    struct comp_collider_render_t {
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
    };

    struct collider_render_system_ctx_t {
        struct vertex_t {
            float x, y; // position
            float r, g, b; // color
        };

        sfk::vertex_buffer_t vertex_buffer;
        sfk::static_index_buffer_t index_buffer;
        sfk::vertex_layout_t vertex_layout;
        sfk::vertex_array_t  vertex_array;
        sfk::program_t program;

        std::vector<vertex_t> mesh;
        uint32_t vertices; // of vertex vvv

        void render_polygon(glm::mat4& vp, comp_collider_render_t* info, b2Body* body, b2PolygonShape* polygon);
        void render_circle(glm::mat4& vp, comp_collider_render_t* info, b2Body* body, b2CircleShape* circle);
    };

    void collider_render_system_on_add(collider_render_system_ctx_t& ctx);
    void collider_render_system_update(flecs::iter& iter, comp_b2Body_t* comp, comp_collider_render_t* tag);
    void collider_render_system_on_remove(collider_render_system_ctx_t& ctx);
    void collider_render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world);
}