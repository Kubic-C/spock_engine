#include "ui_renderer.hpp"
#include "../engine.hpp"

#define MAX_LETTERS 0xfff

const char* vs_font = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_tex_coords;
layout(location = 2) in vec3 a_color;

out vec2 v_tex_coords;
out vec3 v_color;

uniform mat4 u_vp;

void main() {
    gl_Position = u_vp * vec4(a_pos, -0.4f, 1.0);
    v_tex_coords = a_tex_coords;
    v_color = a_color;
}
)###";

const char* fs_font = R"###(
#version 330 core

in vec2 v_tex_coords;
in vec3 v_color;

out vec4 color;

uniform sampler2D font;

void main()
{    
    color = vec4(v_color, texture(font, v_tex_coords).r);
}
)###";

const char* vs_button = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec3 a_color;

out vec3 v_color;

uniform mat4 u_vp;

void main() {
    gl_Position = u_vp * vec4(a_pos, -0.3f, 1.0);
    v_color = a_color;
}
)###";

const char* fs_button = R"###(
#version 330 core

in vec3 v_color;

out vec4 color;

void main()
{    
    color = vec4(v_color, 1.0f);
}
)###"; 

namespace spk {
    void font_render_tt::init() {
        vbo.init(GL_ARRAY_BUFFER);
        vbo.buffer_data(sizeof(vertex_tt) * indexes_per_letter * MAX_LETTERS, nullptr, GL_DYNAMIC_DRAW);

        layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 2 + 3), 0, vbo);
        layout.add(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 2 + 3), sizeof(float) * 2, vbo);
        layout.add(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 2 + 3), sizeof(float) * 4, vbo);
        
        vao.init();
        vao.bind_layout(layout);

        uint32_t vs_shader = sfk::create_shader_from_src(GL_VERTEX_SHADER, vs_font, nullptr);
        uint32_t fs_shader = sfk::create_shader_from_src(GL_FRAGMENT_SHADER, fs_font, nullptr);
        
        assert(vs_shader != UINT32_MAX);
        assert(fs_shader != UINT32_MAX);
        
        program.init();
        DEBUG_VALUE(bool, ret =) program.load_shader_modules(vs_shader, fs_shader);
        assert(ret);

        buffer.resize(indexes_per_letter * MAX_LETTERS);
    }

    void font_render_tt::render(sfk::static_index_buffer_tt& ibo, font_tt* font, ui_canvas_tt* canvas) {
        ui_text_tt* ui_text[24];
        uint32_t texts_found;

        if(!font)
            return;

        texts_found = canvas->texts.get_all_data(ui_text, 24);
        vertex_tt* section = &buffer[0];
        for(uint32_t j = 0; j < texts_found; j++) {
            add_ui_text(section, font, ui_text[j]);
            indexes += indexes_per_letter * ui_text[j]->str.size();
            vertices += vertices_per_letter * ui_text[j]->str.size();
            section += indexes_per_letter * ui_text[j]->str.size();
        }
        
        if(texts_found == 0)
            return;

        // update
        vbo.buffer_sub_data(0, indexes * sizeof(vertex_tt), buffer.data());

        // bind
        vao.bind();
        ibo.bind();
        program.use();
        program.set_mat4("u_vp", canvas->vp);
        program.set_int("font", 0);
        font->texture.active_texture(GL_TEXTURE0);

        // draw
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, nullptr);
        glDisable(GL_BLEND);

        indexes = 0;
        vertices = 0;
    }

    void font_render_tt::free() {
        program.free();
        vbo.free();
        vao.free();
    }

    void font_render_tt::add_ui_text(vertex_tt* vtx, font_tt* font, ui_text_tt* text) {
        float x = text->axises.position.x; // x cursor
        float y = text->axises.position.y;
        float xreset = 0.0f;
        float xwrap = 1000000000000000000000000000000000.0f; // unreasoably high number

        if(text->parent) {
            x = (text->parent->position.x + text->parent->half_width);
            y = (text->parent->position.y + text->parent->half_height);

            // on the first frame, _render.width will be 0
            x -= (text->_render.width / 2.0f);
            y -= (text->_render.height / 2.0f);

            xwrap = text->parent->position.x + text->parent->size.x;
            xreset = x;

            text->_render.width = 0.0f;
            text->_render.height = 0.0f;
        }

        for(u_char c : text->str) {
            character_tt* ch = &font->char_map[c];

            float x2 = x + ch->bearing.x * text->axises.size.x;
            float y2 = y - (ch->size.y - ch->bearing.y) * text->axises.size.y;
            float w = ch->size.x * text->axises.size.x;
            float h = ch->size.y * text->axises.size.y;    

            vtx[0] = { .x = x2,     .y = y2,     .u = ch->tex_indices[0].x, .v = ch->tex_indices[0].y, .r = text->color.r, .g = text->color.g, .b = text->color.b} ;
            vtx[1] = { .x = x2 + w, .y = y2,     .u = ch->tex_indices[1].x, .v = ch->tex_indices[1].y, .r = text->color.r, .g = text->color.g, .b = text->color.b} ;
            vtx[2] = { .x = x2 + w, .y = y2 + h, .u = ch->tex_indices[2].x, .v = ch->tex_indices[2].y, .r = text->color.r, .g = text->color.g, .b = text->color.b} ;
            vtx[3] = { .x = x2,     .y = y2 + h, .u = ch->tex_indices[3].x, .v = ch->tex_indices[3].y, .r = text->color.r, .g = text->color.g, .b = text->color.b} ;
            vtx += 4;

            x += (float)ch->advance * text->axises.size.x;

            text->_render.width += (float)ch->advance * text->axises.size.x;
            text->_render.height = std::max(text->_render.height, h * text->axises.size.y);
        } 
    }

    void button_render_tt::init() {
        vbo.init(GL_ARRAY_BUFFER);
        vbo.buffer_data(sizeof(vertex_tt) * vertices_per_button * MAX_LETTERS, nullptr, GL_DYNAMIC_DRAW);

        layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 3), 0, vbo);
        layout.add(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 3), sizeof(float) * 2, vbo);
        
        vao.init();
        vao.bind_layout(layout);

        uint32_t vs_shader = sfk::create_shader_from_src(GL_VERTEX_SHADER, vs_button, nullptr);
        uint32_t fs_shader = sfk::create_shader_from_src(GL_FRAGMENT_SHADER, fs_button, nullptr);
        
        assert(vs_shader != UINT32_MAX);
        assert(fs_shader != UINT32_MAX);
        
        program.init();
        DEBUG_VALUE(bool, ret =) program.load_shader_modules(vs_shader, fs_shader);
        assert(ret);

        buffer.resize(vertices_per_button * MAX_LETTERS);

        vertices = 0;
    }

    void button_render_tt::render(sfk::static_index_buffer_tt& ibo, ui_canvas_tt* canvas) {
        ui_button_tt* ui_buttons[24];
        uint32_t buttons_found;

        buttons_found = canvas->buttons.get_all_data(ui_buttons, 24);
        vertex_tt* section = &buffer[0];
        for(uint32_t i = 0; i < buttons_found; i++) {
            add_ui_button(section, ui_buttons[i]);
            indexes += indexes_per_button;
            vertices += vertices_per_button;
            section += indexes_per_button;
        }

        if(buttons_found == 0)
            return;

        // update
        vbo.buffer_sub_data(0, vertices * sizeof(vertex_tt), buffer.data());

        // bind
        vao.bind();
        ibo.bind();
        program.use();
        program.set_mat4("u_vp", canvas->vp);

        // draw
        glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, nullptr);

        vertices = 0;
    }

    void button_render_tt::free() {
        vao.free();
        vbo.free();
        program.free();
    }
        
    void button_render_tt::add_ui_button(vertex_tt* vtx, ui_button_tt* btn) {
        glm::vec3 offset = {0.0f, 0.0f, 0.0f};
        glm::vec3 color;

        if(btn->time_when_clicked + 0.1f > glfwGetTime()) {
            offset = {0.1f, 0.1f, 0.1f};
        }

        color = btn->color - offset;

        // bl
        vtx[0] = {.x = btn->axises.position.x,                 .y = btn->axises.position.y, 
                    .r = color.r, .g = color.g, .b = color.b};

        // tr
        vtx[2] = {.x = btn->axises.position.x + btn->axises.size.x,   .y = btn->axises.position.y + btn->axises.size.y, 
                    .r = color.r, .g = color.g, .b = color.b};

        // br
        vtx[1] = {.x = btn->axises.position.x + btn->axises.size.x,   .y = btn->axises.position.y, 
                    .r = color.r, .g = color.g, .b = color.b};

        // tl
        vtx[3] = {.x = btn->axises.position.x,   .y = btn->axises.position.y + btn->axises.size.y, 
                    .r = color.r, .g = color.g, .b = color.b};
    } 

    void ui_renderer_tt::init(scene_tt& scene) {
        font_render.init();
        button_render.init();

        ibo.init(GL_ELEMENT_ARRAY_BUFFER);
        ibo.generate_quad_indexes(MAX_LETTERS);

        canvas = &scene.canvas;
    }

    void ui_renderer_tt::render(scene_tt& scene) {

        button_render.render(ibo, &scene.canvas);
        font_render.render(ibo, scene.engine->resource_manager.get_font(scene.canvas.font), &scene.canvas);
    }

    void ui_renderer_tt::resize(int width, int height) {
        canvas->resize(width, height);
    }

    void ui_renderer_tt::free() {
        ibo.free();
        button_render.free();
        font_render.free();
    }
}