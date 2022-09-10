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
    void font_render_t::init() {
        vbo.init(GL_ARRAY_BUFFER);
        vbo.buffer_data(sizeof(vertex_t) * indexes_per_letter * MAX_LETTERS, nullptr, GL_DYNAMIC_DRAW);

        layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 2 + 3), 0, vbo);
        layout.add(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 2 + 3), sizeof(float) * 2, vbo);
        layout.add(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 2 + 3), sizeof(float) * 4, vbo);
        
        vao.init();
        vao.bind_layout(layout);

        uint32_t vs_shader = sfk::create_shader_from_src(GL_VERTEX_SHADER, vs_font, nullptr);
        uint32_t fs_shader = sfk::create_shader_from_src(GL_FRAGMENT_SHADER, fs_font, nullptr);
        
        sfk_assert(vs_shader != UINT32_MAX);
        sfk_assert(fs_shader != UINT32_MAX);
        
        program.init();
        DEBUG_VALUE(bool, ret =) program.load_shader_modules(vs_shader, fs_shader);
        sfk_assert(ret);

        buffer.resize(indexes_per_letter * MAX_LETTERS);

        indexes = 0;
        vertices = 0;
    }

    void font_render_t::render(sfk::static_index_buffer_t& ibo, font_t* font, ui_canvas_t* canvas) {
        // update
        vbo.buffer_sub_data(0, indexes * sizeof(vertex_t), buffer.data());

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

    void font_render_t::free() {
        program.free();
        vbo.free();
        vao.free();
    }

    void font_render_t::add_ui_text(font_t* font, ui_text_t* text) {
        vertex_t* vtx = buffer.data() + indexes;
        const float scalar = text->text.scalar;
        float xoffset = 0.0f;
        float yoffset = 0.0f;
        float x = text->abs_pos.x; // x cursor
        float y = text->abs_pos.y;

        for(uint8_t c : text->text.str) {
            character_t* ch = &font->char_map[c];

            xoffset += ch->advance * scalar;
            yoffset = std::max(yoffset, ch->size.y * scalar);
        }

        xoffset /= 2.0f;
        yoffset /= 2.0f;
        text->abs_size.x = xoffset;
        text->abs_size.y = yoffset;

        for(uint8_t c : text->text.str) {
            character_t* ch = &font->char_map[c];

            float x2 = x + ch->bearing.x * scalar;
            float y2 = y - (ch->size.y - ch->bearing.y) * scalar;
            float w = ch->size.x * scalar;
            float h = ch->size.y * scalar;    

            vtx[0] = { .x = (x2    ) - xoffset, .y = (y2    ) - yoffset, .uv = ch->tex_indices[0], .rgb = text->text.color} ;
            vtx[1] = { .x = (x2 + w) - xoffset, .y = (y2    ) - yoffset, .uv = ch->tex_indices[1], .rgb = text->text.color} ;
            vtx[2] = { .x = (x2 + w) - xoffset, .y = (y2 + h) - yoffset, .uv = ch->tex_indices[2], .rgb = text->text.color} ;
            vtx[3] = { .x = (x2    ) - xoffset, .y = (y2 + h) - yoffset, .uv = ch->tex_indices[3], .rgb = text->text.color} ;
            vtx += indexes_per_letter;

            x += (float)ch->advance * scalar;
        }

        indexes += indexes_per_letter * text->text.ssize();
        vertices += vertices_per_letter * text->text.ssize();
    }

    void button_render_t::init() {
        vbo.init(GL_ARRAY_BUFFER);
        vbo.buffer_data(sizeof(vertex_t) * vertices_per_button * MAX_LETTERS, nullptr, GL_DYNAMIC_DRAW);

        layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 3), 0, vbo);
        layout.add(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (2 + 3), sizeof(float) * 2, vbo);
        
        vao.init();
        vao.bind_layout(layout);

        uint32_t vs_shader = sfk::create_shader_from_src(GL_VERTEX_SHADER, vs_button, nullptr);
        uint32_t fs_shader = sfk::create_shader_from_src(GL_FRAGMENT_SHADER, fs_button, nullptr);
        
        sfk_assert(vs_shader != UINT32_MAX);
        sfk_assert(fs_shader != UINT32_MAX);
        
        program.init();
        DEBUG_VALUE(bool, ret =) program.load_shader_modules(vs_shader, fs_shader);
        sfk_assert(ret);

        buffer.resize(vertices_per_button * MAX_LETTERS);

        indexes = 0;
        vertices = 0;
    }

    void button_render_t::render(sfk::static_index_buffer_t& ibo, ui_canvas_t* canvas) {
        // update
        vbo.buffer_sub_data(0, vertices * sizeof(vertex_t), buffer.data());

        // bind
        vao.bind();
        ibo.bind();
        program.use();
        program.set_mat4("u_vp", canvas->vp);

        // draw
        glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, nullptr);

        vertices = 0;
        indexes = 0;
    }

    void button_render_t::free() {
        vao.free();
        vbo.free();
        program.free();
    }
        
    void button_render_t::add_ui_button(ui_button_t* btn) {
        vertex_t* vtx = buffer.data() + indexes;
        const float x = btn->abs_pos.x;
        const float y = btn->abs_pos.y;
        const float hw = btn->abs_size.x;
        const float hh = btn->abs_size.y;
        glm::vec3 offset = {0.0f, 0.0f, 0.0f};
        glm::vec3 color;

        if(btn->time_when_clicked + 0.1f > sfk::time.get_time()) {
            offset = {0.5f, 0.5f, 0.5f};
        }

        color = btn->color - offset;
 
        // bl
        vtx[0] = {.x = x - hw, .y = y - hh, 
                    .r = color.r, .g = color.g, .b = color.b};

        // tr
        vtx[2] = {.x = x + hw,   .y = y + hh, 
                    .r = color.r, .g = color.g, .b = color.b};

        // br
        vtx[1] = {.x = x + hw,   .y = y - hh, 
                    .r = color.r, .g = color.g, .b = color.b};

        // tl
        vtx[3] = {.x = x - hw,   .y = y + hh, 
                    .r = color.r, .g = color.g, .b = color.b};

        indexes += indexes_per_button;
        vertices += vertices_per_button;
    } 

    void ui_renderer_t::init(scene_t& scene) {
        font_render.init();
        button_render.init();

        ibo.init(GL_ELEMENT_ARRAY_BUFFER);
        ibo.generate_quad_indexes(MAX_LETTERS);

        canvas = &scene.canvas;
    }

    void ui_renderer_t::render(scene_t& scene) {
        ui_canvas_t& canvas = scene.canvas;
        font_t* font = canvas.font;
        float xmax = canvas.abs_size.x;
        float ymax = canvas.abs_size.y;

        if(!(canvas.flags & UI_ELEMENT_FLAGS_ENABLED) || !canvas.in_use.any())
            return;

        if(!font) { // if font is null, the first font will be the default
            font = scene.engine->resource_manager.get_first_font();
            sfk_assert(font && "when using canvas you must load a font!");
        }

        canvas.iter_children([&](ui_element_t& ele) -> bool {
            if(!(ele.flags & UI_ELEMENT_FLAGS_ENABLED) || ele.flags & UI_ELEMENT_FLAGS_ROOT) {
                return true;
            }

            ui_element_t* parent = ele.parent;

            if(ele.flags & UI_ELEMENT_FLAGS_RELATIVE || 
               (parent->flags & UI_ELEMENT_FLAGS_ROOT && ele.flags & UI_ELEMENT_FLAGS_PARENT_RELATIVE)) {
                ele.gen_abs({-1.0f, 1.0f}, {0, xmax},
                            {-1.0f, 1.0f}, {0, ymax});
            } else if(ele.flags & UI_ELEMENT_FLAGS_PARENT_RELATIVE) {
                glm::vec2 _1abs_pos = {
                    map_value(ele.pos.x, {-1.0f, 1.0f}, {-parent->abs_size.x, parent->abs_size.x}),
                    map_value(ele.pos.y, {-1.0f, 1.0f}, {-parent->abs_size.y, parent->abs_size.y})
                }; 

                ele.abs_pos = _1abs_pos + ele.parent->abs_pos;
                
                if(ele.type() != UI_ELEMENT_TYPE_TEXT) {
                    ele.abs_size = {
                        map_value(ele.size.x, {-1.0f, 1.0f}, {0, xmax}),
                        map_value(ele.size.y, {-1.0f, 1.0f}, {0, ymax})
                    };
                }
            } 

            switch(ele.type()) {
            case UI_ELEMENT_TYPE_ELEMENT:
                break;

            case UI_ELEMENT_TYPE_TEXT:
                font_render.add_ui_text(font, (ui_text_t*)&ele);
                break;

            case UI_ELEMENT_TYPE_BUTTON:
                button_render.add_ui_button((ui_button_t*)&ele);
                break;

            case UI_ELEMENT_TYPE_CANVAS:
                sfk_assert(!"yo dawg how this shit happen, this is not accessible");
                break;

            default:
                break;
            }

            return false;
        });

        button_render.render(ibo, &scene.canvas);
        font_render.render(ibo, font, &scene.canvas);
    }

    void ui_renderer_t::resize(int width, int height) {
        canvas->resize(width, height);
    }

    void ui_renderer_t::free(scene_t& scene) {
        ibo.free();
        button_render.free();
        font_render.free();
    }
}