#include "font_renderer.hpp"
#include "state.hpp"
#include "render_system.hpp"

const char* vs_font = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_tex_coords;
layout(location = 2) in vec3 a_color;

out vec2 v_tex_coords;
out vec3 v_color;

uniform mat4 u_vp;

void main() {
    gl_Position = u_vp * vec4(a_pos, 0.2f, 1.0);
    v_tex_coords = a_tex_coords;
    v_color = a_color;
}
)###";

const char* fs_font = R"###(
#version 330 core
in vec2 v_tex_coords;
in vec3 v_color;

uniform sampler2D font;

out vec4 color;

void main()
{    
    color = vec4(v_color, texture(font, v_tex_coords).r);
}
)###";

namespace spk {
    void font_batch_mesh_t::init() {
        m_init();

        indexes  = 0;
        vertices = 0;

        buffer.resize(indexes_per_letter * SPK_MAX_LETTERS);
        vertex_buffer.buffer_data(sizeof(font_vertex_t) * indexes_per_letter * SPK_MAX_LETTERS, nullptr, GL_DYNAMIC_DRAW);
    }

    void font_batch_mesh_t::zero() {
        indexes  = 0;
        vertices = 0;
    }
    
    void font_batch_mesh_t::free() {
        m_free();   
    }

    void font_batch_mesh_t::subdata() {
        vertex_buffer.buffer_sub_data(0, indexes * sizeof(font_vertex_t), buffer.data());
    }

    void font_batch_mesh_t::add_ui_text(ui_text_t* text) {
        font_vertex_t* vtx = buffer.data() + indexes;
        const float scalar = text->text.scalar;
        float xoffset = 0.0f;
        float yoffset = 0.0f;
        float x = text->abs_pos.x; // x cursor
        float y = text->abs_pos.y;

        for(uint8_t c : text->text.str) {
            character_t* ch = &font->char_map[c];

            xoffset += ch->advance[0] * scalar;
            yoffset = std::max(yoffset, ch->size.y * scalar);
        }

        xoffset /= 2.0f;
        yoffset /= 2.0f;
        text->abs_size.x = xoffset;
        text->abs_size.y = yoffset;

        for(uint8_t c : text->text.str) {
            character_t* ch = &font->char_map[c];

            float x2 = x + ch->offset[0] * scalar;
            float y2 = y - ch->offset[1] * scalar;
            float w = ch->size.x * scalar;
            float h = ch->size.y * scalar;    

            vtx[0] = { .x = (x2    ) - xoffset, .y = (y2    ) - yoffset, .uv = ch->tex_indices[0], .rgb = text->text.color} ;
            vtx[1] = { .x = (x2 + w) - xoffset, .y = (y2    ) - yoffset, .uv = ch->tex_indices[1], .rgb = text->text.color} ;
            vtx[2] = { .x = (x2 + w) - xoffset, .y = (y2 + h) - yoffset, .uv = ch->tex_indices[2], .rgb = text->text.color} ;
            vtx[3] = { .x = (x2    ) - xoffset, .y = (y2 + h) - yoffset, .uv = ch->tex_indices[3], .rgb = text->text.color} ;
            vtx += indexes_per_letter;

            x += (float)ch->advance[0] * scalar;
            y += (float)ch->advance[1] * scalar;
        }

        indexes += indexes_per_letter * text->text.ssize();
        vertices += vertices_per_letter * text->text.ssize();
    }
    
    void font_renderer_t::init() {
        b_init();
        mesh.init();

        vertex_layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 2 + 3), 0, 0);
        vertex_layout.add(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 2 + 3), sizeof(float) * 2, 0);
        vertex_layout.add(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 2 + 3), sizeof(float) * 4, 0);
        
        bool ret = program.load_shader_str(vs_font, fs_font);
        spk_assert(ret);
    }

    void font_renderer_t::free() {
        mesh.free();
        b_free();
    }

    void font_renderer_t::draw() {
        if(!mesh.vertices)
            return;

        mesh.subdata();

        vertex_layout.set_buffer(0, mesh.vertex_buffer);
        vertex_layout.set_buffer(1, mesh.vertex_buffer);
        vertex_layout.set_buffer(2, mesh.vertex_buffer);
        vertex_array.bind_layout(vertex_layout);

        state.get_current_renderer()->quad_index_buffer.bind();
        vertex_array.bind();
        program.use();
        program.set_mat4("u_vp", mesh.vp);
        program.set_int("font", 0);
        mesh.font->texture.active_texture(GL_TEXTURE0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawElements(GL_TRIANGLES, mesh.vertices, GL_UNSIGNED_INT, nullptr);
        glDisable(GL_BLEND);
    }
}