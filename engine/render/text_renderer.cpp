/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "text_renderer.hpp"
#include "core/data.hpp"

namespace spk {
    struct vertex_t {
        glm::vec3 pos;
        glm::vec2 tex;
        glm::vec3 color;
    };

    text_renderer_t::mesh_t::mesh_t() {
        buffer.init(GL_ARRAY_BUFFER);
    }

    text_renderer_t::mesh_t::~mesh_t() {
        buffer.free();
    }

    void text_renderer_t::add_mesh(uint32_t font_id, void* vertices) {
        auto& buffer             = meshes[font_id].buffer; 
        auto& vertexes_on_buffer = meshes[font_id].vertexes_on_buffer;
        auto& vertexes_to_render = meshes[font_id].vertexes_to_render;

        if(buffer.size() < (vertexes_on_buffer + 4) * sizeof(vertex_t)) 
            buffer.resize((vertexes_on_buffer + 4) * sizeof(vertex_t));

        buffer.bind();
        buffer.buffer_sub_data(vertexes_on_buffer * sizeof(vertex_t), sizeof(vertex_t) * 4, vertices);
        vertexes_on_buffer += 4;
        vertexes_to_render += 6;
    }

    void text_renderer_t::text_mesh(const text_t& text) {
        spk_trace();

        font_t&                  font = fonts().get(text.canvas->font);
        const text_dimensions_t& dim  = text.text_dimensions_get(); // this prevents the container from being rendered over text
        const float z      = text.dimensions_get().pos.z + 0.01f;
        glm::vec2   cursor = dim.cursor;

        for(uint8_t c : text.text) {
            character_t& ch = font.char_map[c];

            const float xadvance = (float)ch.advance[0] * dim.scale;
            const float yadvance = (float)ch.advance[1] * dim.scale;

            if(dim.xright < cursor.x + xadvance) {
                cursor.x = dim.xreset;
                cursor.y -= dim.ysub;
            }

            if(cursor.y < dim.ybottom)
                break;

            const float x2 = cursor.x + ch.offset[0] * dim.scale;
            const float y2 = cursor.y - ch.offset[1] * dim.scale;
            const float w  = ch.size.x * dim.scale;
            const float h  = ch.size.y * dim.scale;    

            vertex_t letter[] = {
                { .pos = {(x2    ), (y2    ), z}, .tex = ch.tex_indices[0], .color = text.text_color},
                { .pos = {(x2 + w), (y2    ), z}, .tex = ch.tex_indices[1], .color = text.text_color},
                { .pos = {(x2 + w), (y2 + h), z}, .tex = ch.tex_indices[2], .color = text.text_color},
                { .pos = {(x2    ), (y2 + h), z}, .tex = ch.tex_indices[3], .color = text.text_color}
            };
            
            cursor.x += xadvance;
            cursor.y += yadvance;

            add_mesh(text.canvas->font, letter);
        }
    }

    void text_renderer_t::render() {
        spk_trace();

        auto& shader = render_context().shaders[SHADER_TYPE_TEXT];
        auto& vao    = render_context().common.array;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for(auto& pair : fonts().map()) {
            auto&   mesh  = meshes[pair.first];
            font_t& font  = pair.second;

            if(mesh.vertexes_on_buffer > 0) {
                vao.bind();
                mesh.buffer.bind();
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), nullptr);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, tex));
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, color));
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                glEnableVertexAttribArray(2);

                shader.use();
                shader.set_mat4("u_vp", render_context().ui_camera);
                shader.set_int("font", 0);
                font.texture.bind();
                glActiveTexture(GL_TEXTURE0);

                render_context().common.quad_indexes.bind();

                glDrawElements(GL_TRIANGLES, mesh.vertexes_to_render, GL_UNSIGNED_INT, nullptr);   
                
                mesh.vertexes_on_buffer = 0;
                mesh.vertexes_to_render = 0;
            }
        }

        glDisable(GL_BLEND);
    }
}