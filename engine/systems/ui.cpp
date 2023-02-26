#include "ui.hpp"
#include "state.hpp"
#include "utility/ui.hpp"
#include "window.hpp"
#include "spock.hpp"

namespace spk {
    void ui_render_system_update(flecs::iter& iter, ui_comp_canvas_t* canvas_) {
        auto ctx = get_ctx<ui_meshes_t>(iter);
        ui_comp_canvas_t& canvas = *canvas_;
        font_t* font = canvas.font;
        float xmax = canvas.abs_size.x;
        float ymax = canvas.abs_size.y;

        if(!(canvas.flags & UI_ELEMENT_FLAGS_ENABLED) || !canvas.in_use.any())
            return;

        if(!font) { // if font is null, the first font will be the default
            font = state.engine->rsrc_mng.get_first_font();
            spk_assert(font && "when using canvas you must load a font!");
        }

        ctx->font_mesh->font = font;
        ctx->font_mesh->vp   = canvas.vp;
        ctx->font_mesh->zero();

        ctx->btn_mesh->vp    = canvas.vp;
        ctx->btn_mesh->zero();

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
                ctx->font_mesh->add_ui_text((ui_text_t*)&ele);
                break;

            case UI_ELEMENT_TYPE_BUTTON:
                ctx->btn_mesh->add_ui_button((ui_button_t*)&ele);
                break;

            case UI_ELEMENT_TYPE_CANVAS:
                spk_assert(!"a canvas within a canvas is not allowed");
                break;

            default:
                break;
            }

            return false;
        });
    }

    void ui_render_system_resize(flecs::iter& iter) {
        auto canvas = state.get_current_canvas().get_ref<ui_comp_canvas_t>();
        event_window_size_t* resize = iter.param<event_window_size_t>();

        canvas->resize_callback(resize->width, resize->height);
    }

    void ui_system_mouse_callback(flecs::iter& iter) {
        auto window = state.get_current_window().get_ref<comp_window_t>();
        auto canvas = state.get_current_canvas().get_ref<ui_comp_canvas_t>();
        const event_window_mouse_click_t* mouse_info = iter.param<event_window_mouse_click_t>();
        glm::ivec2 size = window->get_size();
        float x, y;

        // in a ui_canvas coord system, (0,0) starts at the bottom left 
        // corner in the window, SDL sends us coordinates oriented around the top left
        x = mouse_info->x;
        y = size.y - mouse_info->y;

        if(mouse_info->button == SDL_BUTTON_LEFT && mouse_info->state == SDL_PRESSED) {
            // iterate through all blocks and use check_button lambda on them

            for(auto& btn : canvas->btns) {
                glm::vec2 corners[2] = {
                    btn.abs_pos - btn.abs_size,
                    btn.abs_pos + btn.abs_size
                };

                if(!(btn.flags & spk::UI_ELEMENT_FLAGS_ENABLED))
                    continue;

                if(corners[0].x < x && x < corners[1].x &&
                    corners[0].y < y && y < corners[1].y) {
                    
                    btn.time_when_clicked = spk::time.get_time();

                    if(btn.callback) {
                        btn.callback(*state.engine, btn);
                    }

                    break;
                }
            }
        }
    }

    void ui_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        spk_trace();
        
        ui_canvas_comp_init(world);

        auto rs            = state.get_current_renderer();
        auto ui_ctx        = ctx_alloc.allocate_ctx<ui_system_ctx_t>();
        auto ui_meshes     = ctx_alloc.allocate_ctx<ui_meshes_t>();
        auto font_renderer = ctx_alloc.allocate_ctx<font_renderer_t>();
        auto btn_renderer  = ctx_alloc.allocate_ctx<button_renderer_t>();
        auto fb_renderer   = ctx_alloc.allocate_ctx<ui_framebuffer_renderer_t>();

        // ORDER MATTERS HERE, buttons will render first, then the font renderer
        id_t ui_fb = rs->fb_init();    
        rs->fb_set_clear_bits(ui_fb, GL_COLOR_BUFFER_BIT);
        rs->fb_set_clear_color(ui_fb, 0.0f, 0.0f, 0.0f, 0.0f);

        id_t color         = rs->atch_init();
        rs->atch_set(color, GL_COLOR_ATTACHMENT0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
        rs->fb_attach(ui_fb, color);

        id_t rp = rs->rp_init();
        rs->rp_set_fb(rp, ui_fb);
        rs->rp_add_renderer(rp, (base_renderer_t*)btn_renderer);
        rs->rp_add_renderer(rp, (base_renderer_t*)font_renderer);
        rs->rp_set_fb_renderer(rp, (base_framebuffer_renderer_t*)fb_renderer);

        ui_meshes->font_mesh = &font_renderer->mesh;
        ui_meshes->btn_mesh  = &btn_renderer->mesh;

        world.system<ui_comp_canvas_t>()
            .term<ui_tag_current_canvas_t>()
            .kind(on_mesh).ctx(ui_meshes).iter(ui_render_system_update).add<tag_render_system_t>();
        
        world.observer().event<event_window_size_t>().term<tag_events_t>()
            .iter(ui_render_system_resize);

        world.observer().event<event_window_mouse_click_t>().term<tag_events_t>()
            .iter(ui_system_mouse_callback);
    }
}