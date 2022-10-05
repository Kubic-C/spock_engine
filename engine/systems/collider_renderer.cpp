#include "collider_renderer.hpp"
#include "renderer.hpp"
#include <glm/gtx/rotate_vector.hpp> 
 
const char* vs_colliders = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec3 a_color;

out vec3 v_color;

uniform mat4 u_vp;

void main() {
    gl_Position = u_vp * vec4(a_pos, 0.0, 1.0);
    v_color = a_color;
})###";

const char* fs_colliders = R"###(
#version 330 core
in vec3 v_color;

out vec4 fragment_color;

void main() {
    fragment_color = vec4(v_color.xyz, 1.0);
}
})###";
 
namespace spk {
    void collider_renderer_t::init(scene_t& scene) {

        { // vertex data
            vertex_array.init();
            vertex_array.bind();
    
            vertex_buffer.init(GL_ARRAY_BUFFER);
            vertex_buffer.buffer_data(sizeof(float) * 2 * 3 * 6 * 100, nullptr, GL_DYNAMIC_DRAW);
            vertex_layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0, vertex_buffer);
            vertex_layout.add(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 2, vertex_buffer);
            vertex_array.bind_layout(vertex_layout);

            index_buffer.init(GL_ELEMENT_ARRAY_BUFFER);
            index_buffer.generate_quad_indexes(4);
        }
    
        uint32_t vs_shader = sfk::create_shader_from_src(GL_VERTEX_SHADER, vs_colliders, nullptr);
        uint32_t fs_shader = sfk::create_shader_from_src(GL_FRAGMENT_SHADER, fs_colliders, nullptr);
        
        sfk_assert(vs_shader != UINT32_MAX);
        sfk_assert(fs_shader != UINT32_MAX);
        
        program.init();
        DEBUG_VALUE(bool, ret =) program.load_shader_modules(vs_shader, fs_shader);
        sfk_assert(ret);
    
        mesh.resize(100 * 3);
    }
    
    void collider_renderer_t::render(scene_t& scene) {
        flecs::world& world = scene.world;
        q = world.query<comp_b2Body, collider_render_t>();

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        q.iter([&](flecs::iter& it, comp_b2Body* c_bodies, collider_render_t* c_primitives) {
            for(auto i : it) {
                collider_render_t* primitive = &c_primitives[i];
                b2Body* body = c_bodies[i].body;
                b2Fixture* fixture = body->GetFixtureList();

                sfk_assert(body);

                for(; fixture; fixture = fixture->GetNext()) {
                    b2Shape* shape = fixture->GetShape();
                    b2Shape::Type type = shape->GetType();

                    switch(type) {
                    case b2Shape::Type::e_polygon:
                        render_polygon(scene.render_scene->vp, primitive, body, (b2PolygonShape*)shape);
                        break;

                    case b2Shape::Type::e_circle:
                        render_circle(scene.render_scene->vp, primitive, body, (b2CircleShape*)shape);
                        break;

                    default:
                        sfk::log.log(sfk::LOG_TYPE_INFO, "unsupported collider type for primitive renderer");
                    };
                }
            }
        });

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    void collider_renderer_t::render_polygon(glm::mat4& vp, collider_render_t* primitive, b2Body* body, b2PolygonShape* polygon) {
        int32_t count = (polygon->m_count / 2) * 3; // special case: 3 works because of integer division black magic

        for(int32 j = 0; j < polygon->m_count; j++)  {
            mesh[j].x = body->GetWorldPoint(polygon->m_vertices[j]).x;
            mesh[j].y = body->GetWorldPoint(polygon->m_vertices[j]).y;
            mesh[j].r = primitive->color.r;
            mesh[j].g = primitive->color.g;
            mesh[j].b = primitive->color.b;
        }

        vertex_buffer.buffer_sub_data(0, polygon->m_count * sizeof(vertex_t), mesh.data());
        vertex_array.bind();

        index_buffer.bind();
        program.use();
        program.set_mat4("u_vp", vp);
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);   
    }

    void collider_renderer_t::render_circle(glm::mat4& vp, collider_render_t* primitve, b2Body* body, b2CircleShape* circle) {
        const glm::vec2 position = sfk::to_glm_vec2(body->GetPosition());
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

        vertex_t* section = mesh.data();
        for(uint32_t i = 0; i < steps; i++) {
            glm::vec2 next = (glm::vec2){ (cos(angle) * r), (sin(angle) * r) };

            // add rotation
            next = glm::rotate(next, body_angle) + position;

            section[0] = { position.x, position.y,  primitve->color.r, primitve->color.g, primitve->color.b }; // center
            section[1] = { prev.x,     prev.y,      primitve->color.r, primitve->color.g, primitve->color.b };
            section[2] = { next.x,     next.y,      primitve->color.r, primitve->color.g, primitve->color.b };

            angle += c_sect;
            prev = next;
            section += 3;
        }

        vertex_buffer.buffer_sub_data(0, vertices * sizeof(vertex_t), mesh.data());
        vertex_array.bind();

        program.use();
        program.set_mat4("u_vp", vp);
        glDrawArrays(GL_TRIANGLES, 0, vertices);
    }
 
    void collider_renderer_t::resize(int width, int height) {
    }
 
    void collider_renderer_t::free(scene_t& scene) {
        vertex_array.free();
        vertex_buffer.free();
        program.free();
    }
}
