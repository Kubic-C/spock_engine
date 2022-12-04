#include "ui.hpp"
#include "../state.hpp"
#include "../utility/ui.hpp"
#include "window.hpp"
#include "primitive_render.hpp"
#include "../spock.hpp"
#include <glm/gtc/matrix_transform.hpp>

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

    void font_render_t::render(sfk::static_index_buffer_t& ibo, font_t* font, ui_comp_canvas_t* canvas) {
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

    void button_render_t::render(sfk::static_index_buffer_t& ibo, ui_comp_canvas_t* canvas) {
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

    void ui_render_system_update(flecs::iter& iter, ui_comp_canvas_t* canvas_) {
        auto ctx = SPK_GET_CTX_REF(iter, ui_render_system_ctx_t);
        ui_comp_canvas_t& canvas = *canvas_;
        font_t* font = canvas.font;
        float xmax = canvas.abs_size.x;
        float ymax = canvas.abs_size.y;

        if(!(canvas.flags & UI_ELEMENT_FLAGS_ENABLED) || !canvas.in_use.any())
            return;

        if(!font) { // if font is null, the first font will be the default
            font = state.engine->rsrc_mng.get_first_font();
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
                ctx->font_render.add_ui_text(font, (ui_text_t*)&ele);
                break;

            case UI_ELEMENT_TYPE_BUTTON:
                ctx->button_render.add_ui_button((ui_button_t*)&ele);
                break;

            case UI_ELEMENT_TYPE_CANVAS:
                sfk_assert(!"yo dawg how this shit happen, this is not accessible");
                break;

            default:
                break;
            }

            return false;
        });

        ctx->button_render.render(ctx->ibo, canvas_);
        ctx->font_render.render(ctx->ibo, font, canvas_);
    }

    void ui_render_system_resize(flecs::iter& iter) {
        auto canvas = state._get_current_canvas().get_ref<ui_comp_canvas_t>();
        event_window_size_t* resize = iter.param<event_window_size_t>();

        canvas->resize_callback(resize->width, resize->height);
    }

    void ui_system_mouse_callback(flecs::iter& iter) {
        auto window = state._get_current_window().get_ref<comp_window_t>();
        auto canvas = state._get_current_canvas().get_ref<ui_comp_canvas_t>();
        const event_window_mouse_click_t* mouse_info = iter.param<event_window_mouse_click_t>();
        glm::ivec2 size = window->get_size();
        float x, y;

        // in a ui_canvas coord system, (0,0) starts at the bottom left 
        // corner in the window, SDL sends us coordinates oriented around the top left
        x = mouse_info->x;
        y = size.y - mouse_info->y;

        if(mouse_info->button == SDL_BUTTON_LEFT && mouse_info->state == SDL_PRESSED) {
            auto check_button = [&](ui_button_t& btn) {
                    glm::vec2 corners[2] = {
                        btn.abs_pos - btn.abs_size,
                        btn.abs_pos + btn.abs_size
                    };

                    if(!(btn.flags & spk::UI_ELEMENT_FLAGS_ENABLED))
                        return false;

                    if(corners[0].x < x && x < corners[1].x &&
                        corners[0].y < y && y < corners[1].y) {
                        
                        btn.time_when_clicked = sfk::time.get_time();

                        if(btn.callback) {
                            btn.callback(*state.engine, btn);
                        }

                        return true;
                    }

                    return false;
            };

            canvas->btns.get_valid_blocks(nullptr, UINT32_MAX, check_button);
        }
    }

    void ui_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        ui_canvas_init(world);

        world.component<ui_system_ctx_t>();
        sfk_register_component(world, ui_render_system_ctx_t);

        auto ui_ctx = ctx_alloc.allocate_ctx<ui_system_ctx_t>();
        auto ui_render_ctx = ctx_alloc.allocate_ctx<ui_render_system_ctx_t>();

        world.system<ui_comp_canvas_t>()
            .term_at(1).with<ui_tag_current_canvas_t>()
            .kind(flecs::OnUpdate)
            .ctx(ui_render_ctx).iter(ui_render_system_update);
        
        world.observer().event<event_window_size_t>().term<tag_events_t>()
            .iter(ui_render_system_resize);

        world.observer().event<event_window_mouse_click_t>().term<tag_events_t>()
            .iter(ui_system_mouse_callback);
    }
}