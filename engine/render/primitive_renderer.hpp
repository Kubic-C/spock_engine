#pragma once

#include "base_renderer.hpp"
#include "components/box2D.hpp"
#include "components/tilebody.hpp"

/* quick and dirty debug rendering for Box2D shapes or primitive shapes */

namespace spk {
    struct comp_body_prim_t {
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
    };

    struct comp_box_t {
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        glm::vec2 position;
        glm::vec2 size;
    };

    struct prim_vertex_t {
        glm::vec2 pos; // position
    };

    class primitive_renderer_t : public base_renderer_t {
    public:
        void init() override;
        void draw() override;
        void free() override;

        void render_box(glm::mat4& vp, static_index_buffer_t& ind, comp_body_prim_t* info, const comp_box_t* box);
        void render_polygon(glm::mat4& vp, static_index_buffer_t& ind, comp_body_prim_t* info, b2Body* body, b2PolygonShape* polygon);
        void render_circle(glm::mat4& vp, comp_body_prim_t* info, b2Body* body, b2CircleShape* circle);
        void render_edge(glm::mat4& vp, comp_body_prim_t* info, b2Body* body, b2EdgeShape* circle);
        void draw_indexed_buffer(glm::vec3 color, static_index_buffer_t& buffer, glm::mat4& vp, uint32_t count);
        void draw_buffer(glm::vec3 color, glm::mat4& vp, uint32_t count);

    private:
        vertex_buffer_t            vertex_buffer;
        std::vector<prim_vertex_t> mesh;
    };
}