#include "primitive_renderer.hpp"
#include "../utility/physics_.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp> 
 
uint32_t index_a[] = {
   0, 1, 2,
   1, 2, 3
};
 
namespace spk {
    void primitive_renderer_t::init(scene_t& scene) {

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
    
        program.init();
        if(!program.load_shader_files("./shaders/vs_tiles.glsl", "./shaders/fs_tiles.glsl")) {
            printf("couldn't load primitve shaders\n");
            abort();
        }
    
        mesh.resize(100 * 3);
    }
    
    void primitive_renderer_t::render(scene_t& scene) {
        flecs::world& world = scene.world;
        auto q = world.query<comp_b2Body, primitive_render_t>();
        
        q.iter([&](flecs::iter& it, comp_b2Body* c_bodies, primitive_render_t* c_primitives) {
            for(auto i : it) {
                primitive_render_t* primitive = &c_primitives[i];
                b2Body* body = c_bodies[i].body;
                b2Fixture* fixture = body->GetFixtureList();

                assert(body);

                for(; fixture; fixture = fixture->GetNext()) {
                    b2Shape* shape = fixture->GetShape();
                    b2Shape::Type type = shape->GetType();
                    int32_t count = 0;

                    switch(type) {
                    case b2Shape::Type::e_polygon:
                        render_polygon(primitive, body, (b2PolygonShape*)shape);
                        break;

                    case b2Shape::Type::e_circle:
                        render_circle(primitive, body, (b2CircleShape*)shape);
                        break;

                    default:
                        abort();
                    };
                }
            }
        });

    }
    
    void primitive_renderer_t::render_polygon(primitive_render_t* primitive, b2Body* body, b2PolygonShape* polygon) {
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

    void primitive_renderer_t::render_circle(primitive_render_t* primitve, b2Body* body, b2CircleShape* circle) {
        const glm::vec2 position = sfk::to_box_vec2(body->GetPosition());
        const uint32_t steps = 6;
        const float r = circle->m_radius;
        const float c = 2.0f * r * b2_pi;
        const float c_sect = (2.0f * b2_pi) / steps; // returns in radians how much we should rotate
        float body_angle = body->GetAngle();
        float angle = c_sect;

        // we are starting from the right of the circle, so cos = r
        // the Y will equal 0 when cos = r
        glm::vec2 prev = (glm::vec2){ r, 0.0f };
        prev = glm::rotate(prev, body_angle) + position;

        uint32_t vertices = steps * 3; // steps is the amount of triangles, each TRIangle has three vertices

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
 
    void primitive_renderer_t::resize(int width, int height) {
        float half_width  = (float)width / 4;
        float half_height = (float)height / 4;
    
        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        proj = glm::ortho(-(float)half_width / sfk::ppm, (float)half_width / sfk::ppm, -half_height / sfk::ppm, half_height / sfk::ppm);
        vp   = proj * view;
    }
 
    void primitive_renderer_t::free() {
        vertex_array.free();
        vertex_buffer.free();
        program.free();
    }
}
