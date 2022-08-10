#include "primitive_renderer.hpp"
#include "../utility/physics_.hpp"
#include <glm/gtc/matrix_transform.hpp>
 
uint32_t index_a[] = {
   0, 1, 2,
   1, 2, 3
};
 
namespace spk {
    void primitive_renderer_tt::init(scene_tt& scene) {

        { // vertex data
            vertex_array.init();
            vertex_array.bind();
    
            vertex_buffer.init(GL_ARRAY_BUFFER);
            vertex_buffer.buffer_data(sizeof(float) * 2 * 3 * 6, nullptr, GL_DYNAMIC_DRAW);
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
    
        mesh.resize(24);
    }
    
    void primitive_renderer_tt::render(scene_tt& scene) {
        flecs::world& world = scene.world;
        auto q = world.query<comp_b2Body, primitive_render_tt>();
        
        q.iter([&](flecs::iter& it, comp_b2Body* c_bodies, primitive_render_tt* c_primitives) {
            for(auto i : it) {
                primitive_render_tt* primitive = &c_primitives[i];
                b2Body* body = c_bodies[i].body;
                b2Fixture* fixture = body->GetFixtureList();

                assert(body);

                for(; fixture; fixture = fixture->GetNext()) {
                    b2Shape* shape = fixture->GetShape();
                    b2PolygonShape* polygon = nullptr;
                    b2Shape::Type type = shape->GetType();
                    int32_t count = 0;

                    if(type != b2Shape::Type::e_polygon) 
                        abort(); // silly goofy app killing
                    else
                        polygon = (b2PolygonShape*)shape;


                    count = (polygon->m_count / 2) * 3; // special case: 3 works because of integer division black magic

                    for(int32 j = 0; j < polygon->m_count; j++)  {
                        mesh[j].x = body->GetWorldPoint(polygon->m_vertices[j]).x;
                        mesh[j].y = body->GetWorldPoint(polygon->m_vertices[j]).y;
                        mesh[j].r = primitive->color.r;
                        mesh[j].g = primitive->color.g;
                        mesh[j].b = primitive->color.b;
                    }

                    vertex_buffer.buffer_sub_data(0, polygon->m_count * sizeof(vertex_tt), mesh.data());
                    vertex_array.bind();

                    index_buffer.bind();
                    program.use();
                    program.set_mat4("u_vp", vp);
                    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
                }
            }
        });

    }
 
    void primitive_renderer_tt::resize(int width, int height) {
        float half_width  = (float)width / 2;
        float half_height = (float)height / 2;
    
        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        proj = glm::ortho(-(float)half_width / sfk::ppm, (float)half_width / sfk::ppm, -half_height / sfk::ppm, half_height / sfk::ppm);
        vp   = proj * view;
    }
 
    void primitive_renderer_tt::free() {
        vertex_array.free();
        vertex_buffer.free();
        program.free();
    }
}
