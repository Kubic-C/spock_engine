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

    void text_renderer_t::text_mesh(glm::vec3 pos, glm::vec2 size, const text_t& text) {
        spk_trace();

        font_t& font = fonts().get(text.canvas->font);

        // this prevents the container from being rendered over text
        const float z = pos.z + 0.01f;
        float scalar  = 1.0f;
        float xoffset = 0.0f;
        float yoffset = 0.0f;
        float x = pos.x; // x cursor
        float y = pos.y;

        for(uint8_t c : text.text) {
            character_t* ch = &font.char_map[c];

            xoffset += ch->advance[0] * scalar;
            yoffset = std::max(yoffset, ch->size.y * scalar);
        }

        xoffset /= 2.0f;
        yoffset /= 2.0f;

        for(uint8_t c : text.text) {
            character_t& ch = font.char_map[c];

            float x2 = x + ch.offset[0] * scalar;
            float y2 = y - ch.offset[1] * scalar;
            float w = ch.size.x * scalar;
            float h = ch.size.y * scalar;    

            vertex_t letter[] = {
                { .pos = {(x2    ) - xoffset, (y2    ) - yoffset, z}, .tex = ch.tex_indices[0], .color = text.text_color},
                { .pos = {(x2 + w) - xoffset, (y2    ) - yoffset, z}, .tex = ch.tex_indices[1], .color = text.text_color},
                { .pos = {(x2 + w) - xoffset, (y2 + h) - yoffset, z}, .tex = ch.tex_indices[2], .color = text.text_color},
                { .pos = {(x2    ) - xoffset, (y2 + h) - yoffset, z}, .tex = ch.tex_indices[3], .color = text.text_color}
            };

            x += (float)ch.advance[0] * scalar;
            y += (float)ch.advance[1] * scalar;

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