#include "tile_renderer.hpp"
#include "renderer.hpp"

const char* vs_tiles = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_tex_coords;

out vec2 v_tex_coords;

uniform mat4 u_vp;

void main() {
    gl_Position = u_vp * vec4(a_pos, 0.0, 1.0);
    v_tex_coords = a_tex_coords;
}
)###";

const char* fs_tiles = R"###(
#version 330 core
in vec2 v_tex_coords;

uniform sampler2D image;
out vec4 fragment_color;

void main() {
    fragment_color = texture(image, v_tex_coords);
}
)###";

namespace spk {
    void tile_renderer_t::init(scene_t& scene) {
        const uint32_t max_tiles = 100;

        {
            vertex_array.init();
            vertex_array.bind();

            vertex_buffer.init(GL_ARRAY_BUFFER);
            vertex_buffer.buffer_data(sizeof(float) * 2 * 2 * 6 * max_tiles, nullptr, GL_DYNAMIC_DRAW);
            vertex_layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0, vertex_buffer);
            vertex_layout.add(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, sizeof(float) * 2, vertex_buffer);
            vertex_array.bind_layout(vertex_layout);

            index_buffer.init(GL_ELEMENT_ARRAY_BUFFER);
            index_buffer.generate_quad_indexes(max_tiles);
        }

        {
            uint32_t vsh, fsh;

            vsh = sfk::create_shader_from_src(GL_VERTEX_SHADER, vs_tiles, nullptr);
            fsh = sfk::create_shader_from_src(GL_FRAGMENT_SHADER, fs_tiles, nullptr);
        
            sfk_assert(vsh != UINT32_MAX);
            sfk_assert(fsh != UINT32_MAX);

            program.init();
            DEBUG_VALUE(bool, ret =) program.load_shader_modules(vsh, fsh);
            sfk_assert(ret);
        }

        mesh.resize(4 * max_tiles);
    }

    void tile_renderer_t::render(scene_t& scene) {
        tile_dictionary_t* dict = scene.tile_dict;
        if(dict == nullptr)
            return;
    
        q = scene.world.query<tile_render_t, tile_body_t>();
    

        q.iter([&](flecs::iter& it, tile_render_t* c_render, tile_body_t* c_bodies) {
            for(auto i : it) {
                tile_render_t& render = c_render[i];
                tile_body_t& body = c_bodies[i];
                glm::vec2 position = sfk::to_glm_vec2(body.body->GetPosition());
                uint32_t vertices = 0;
                uint32_t indexes = 0;

                for(uint32_t x = 0; x < body.tiles.size(); x++) {
                    for(uint32_t y = 0; y < body.tiles[x].size(); y++) {
                        if(body.tiles[x][y].type_id != sfk::MAX_KEY) {
                            const uint32_t type_id = body.tiles[x][y].type_id;
                            const float half_size = 0.5f;
                            vertex_t* begin = mesh.data() + vertices; 
                            glm::vec2 tile_pos = position + glm::vec2(x, y);

                            begin[0] = { .x = tile_pos.x - half_size, .y = tile_pos.y - half_size, .u = dict->render[type_id].tex_coords[0].x, .v = dict->render[type_id].tex_coords[0].y};
                            begin[1] = { .x = tile_pos.x + half_size, .y = tile_pos.y - half_size, .u = dict->render[type_id].tex_coords[1].x, .v = dict->render[type_id].tex_coords[1].y};
                            begin[2] = { .x = tile_pos.x + half_size, .y = tile_pos.y + half_size, .u = dict->render[type_id].tex_coords[2].x, .v = dict->render[type_id].tex_coords[2].y};
                            begin[3] = { .x = tile_pos.x - half_size, .y = tile_pos.y + half_size, .u = dict->render[type_id].tex_coords[3].x, .v = dict->render[type_id].tex_coords[3].y};

                            vertices += 4;
                            indexes += 6;
                        }
                    }
                }

                vertex_buffer.buffer_sub_data(0, mesh.size() * sizeof(vertex_t), mesh.data());
                vertex_array.bind();

                index_buffer.bind();

                program.use();
                program.set_mat4("u_vp", scene.render_scene->vp);
                program.set_int("image", 0);
                dict->texture.active_texture(GL_TEXTURE0);
                
                glDrawElements(GL_TRIANGLES, indexes, GL_UNSIGNED_INT, nullptr);  
            }
        });
    }

    void tile_renderer_t::resize(int width, int height) {

    }

    void tile_renderer_t::free(scene_t& scene) {
        vertex_array.free();
        vertex_buffer.free();
        index_buffer.free();
        program.free();
    }

    void tile_renderer_t::dirty_rendering(glm::mat4& vp, collider_render_t* primitive, b2Body* body, b2PolygonShape* polygon) {
        int32_t count = (polygon->m_count / 2) * 3; // special case: 3 works because of integer division black magic
        struct {
            float x, y;
            float r, g, b;
        } mesh[4];

        for(int32 j = 0; j < polygon->m_count; j++)  {
            mesh[j].x = body->GetWorldPoint(polygon->m_vertices[j]).x;
            mesh[j].y = body->GetWorldPoint(polygon->m_vertices[j]).y;
            mesh[j].r = primitive->color.r;
            mesh[j].g = primitive->color.g;
            mesh[j].b = primitive->color.b;
        }

        vertex_buffer.buffer_sub_data(0, polygon->m_count * sizeof(float) * 5, mesh);
        vertex_array.bind();

        index_buffer.bind();
        program.use();
        program.set_mat4("u_vp", vp);
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);  
    }

}