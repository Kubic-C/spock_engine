#include "primitive_renderer.hpp"
#include "state.hpp"
#include "spock.hpp"
#include <glm/gtx/rotate_vector.hpp> 

const char* vs_colliders = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;

uniform mat4 u_vp;

void main() {
    gl_Position = u_vp * vec4(a_pos, 0.0, 1.0);
})###";

const char* fs_colliders = R"###(
#version 330 core

uniform vec3 color;

out vec4 fragment_color;

void main() {
    fragment_color = vec4(color.xyz, 1.0);
})###";

namespace spk {
    void primitive_renderer_t::draw_indexed_buffer(glm::vec3 color, static_index_buffer_t& ind, glm::mat4& vp, uint32_t count) { 
        ind.bind();
        program.use();
        program.set_mat4("u_vp", vp);
        program.set_vec3("color", color);
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);   
    }

    void primitive_renderer_t::draw_buffer(glm::vec3 color, glm::mat4& vp, uint32_t count) { 
        program.use();
        program.set_mat4("u_vp", vp);
        program.set_vec3("color", color);
        glDrawArrays(GL_TRIANGLES, 0, count);   
    }

    void primitive_renderer_t::render_box(glm::mat4& vp, spk::static_index_buffer_t& ind, 
        comp_body_prim_t* ri, const comp_box_t* box) {
        const uint32_t quad_vert_count = 4;
        const uint32_t quad_index_count = 6;
        
        mesh[0].pos = { box->position.x,               box->position.y };
        mesh[1].pos = { box->position.x + box->size.x, box->position.y };
        mesh[2].pos = { box->position.x + box->size.x, box->position.y + box->size.y };
        mesh[3].pos = { box->position.x,               box->position.y + box->size.y };

        vertex_buffer.buffer_sub_data(0, quad_vert_count * sizeof(prim_vertex_t), mesh.data());
        vertex_array.bind();

        draw_indexed_buffer(ri->color, ind, vp, quad_index_count);
    }

    void primitive_renderer_t::render_polygon(glm::mat4& vp, spk::static_index_buffer_t& ind, 
        comp_body_prim_t* ri, b2Body* body, b2PolygonShape* polygon) {
        // this will get the amount of indexes to use, so if quad has 4 verts: 4 / 2 = 2 -> 2 * 3 = 6 (indexes)
        int32_t index_count = (polygon->m_count / 2) * 3; // special case: 3 works because of integer division black magic

        for(int32 j = 0; j < polygon->m_count; j++)  {
            mesh[j].pos = { 
                body->GetWorldPoint(polygon->m_vertices[j]).x, body->GetWorldPoint(polygon->m_vertices[j]).y };
        }

        vertex_buffer.buffer_sub_data(0, polygon->m_count * sizeof(prim_vertex_t), mesh.data());
        vertex_array.bind();
        
        draw_indexed_buffer(ri->color, ind, vp, index_count);
    }

    void primitive_renderer_t::render_circle(glm::mat4& vp, comp_body_prim_t* ri, b2Body* body, b2CircleShape* circle) {
        const glm::vec2 position = spk::to_glm_vec2(body->GetPosition());
        const uint32_t steps = 8;
        const float r = circle->m_radius;
        const float c = 2.0f * r * b2_pi;
        const float c_sect = (2.0f * b2_pi) / steps; // returns in radians how much we should rotate
        float body_angle = body->GetAngle();
        float angle = c_sect;

        // we are starting from the right of the circle, so cos = r
        // the Y will equal 0 when cos = r
        glm::vec2 prev = (glm::vec2){ r, 0.0f };
        prev = glm::rotate(prev, body_angle) + position;

        uint32_t vertices = steps * 3; // steps is the amount of triangles, each TRI-angle has three vertices

        prim_vertex_t* section = mesh.data();
        for(uint32_t i = 0; i < steps; i++) {
            glm::vec2 next = (glm::vec2){ (cos(angle) * r), (sin(angle) * r) };

            // add rotation
            next = glm::rotate(next, body_angle) + position;

            section[0] = { position }; // center
            section[1] = { prev     };
            section[2] = { next     };

            angle += c_sect;
            prev = next;
            section += 3;
        }

        vertex_buffer.buffer_sub_data(0, vertices * sizeof(prim_vertex_t), mesh.data());
        vertex_array.bind();

        draw_buffer(ri->color, vp, vertices);
    }

    void primitive_renderer_t::render_edge(glm::mat4& vp, comp_body_prim_t* ri, b2Body* body, b2EdgeShape* edge) {
        uint32_t vertices = 2;

        mesh[0] = { spk::to_glm_vec2(body->GetWorldPoint(edge->m_vertex1)) };
        mesh[1] = { spk::to_glm_vec2(body->GetWorldPoint(edge->m_vertex2)) };
    
        vertex_buffer.buffer_sub_data(0, vertices * sizeof(prim_vertex_t), mesh.data());
        vertex_array.bind();

        draw_buffer(ri->color, vp, vertices);
    }

    void primitive_renderer_t::init() {
        b_init();

        vertex_buffer.init(GL_ARRAY_BUFFER);
        vertex_buffer.buffer_data(sizeof(prim_vertex_t) * 4 * 100, nullptr, GL_DYNAMIC_DRAW);
        vertex_layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0, vertex_buffer);
        vertex_array.bind_layout(vertex_layout);

        bool ret = program.load_shader_str(vs_colliders, fs_colliders);
        spk_assert(ret && "primitive render shaders invalid");
    
        mesh.resize(100 * 3);
    }

    void primitive_renderer_t::draw() {
        auto world = state.engine->get_current_b2World();

        // should probably add this but im lazzzyy
        world->DebugDraw();
    }

    void primitive_renderer_t::free() {
        b_free();
    }
}