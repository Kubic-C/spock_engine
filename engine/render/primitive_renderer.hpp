#pragma once

#include "base_renderer.hpp"
#include "components/box2D.hpp"

/* quick and dirty debug rendering for Box2D shapes or primitive shapes */

namespace spk {
    struct comp_body_prim_t {};

    struct prim_vertex_t {
        glm::vec2 pos; // position
    };

    struct polygon_batch_mesh_t : public mesh_t {        
        std::vector<prim_vertex_t> buffer;

        glm::mat4 vp;

        uint32_t count;

        void init();
        void free();
        void subdata();
        void zero();
        
        void add_polygon(const b2Body* body, b2PolygonShape* polygon);
    };
    class primitive_renderer_t : public base_renderer_t {
    public:
        void init() override;
        void draw() override;
        void free() override;

        void render_circle(glm::mat4& vp, const b2Body* body, b2CircleShape* circle);
        void render_edge(glm::mat4& vp, const b2Body* body, b2EdgeShape* circle);
        void draw_indexed_buffer(static_index_buffer_t& buffer, glm::mat4& vp, uint32_t count);
        void draw_buffer(glm::mat4& vp, uint32_t count);

        polygon_batch_mesh_t poly_mesh;
        glm::vec3            color;

    private:
        vertex_buffer_t            vertex_buffer;
        std::vector<prim_vertex_t> mesh;
    };
}