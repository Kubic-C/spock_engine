#include "ui.hpp"
#include "../state.hpp"
#include "../utility/ui_.hpp"
#include "window.hpp"
#include "primitive_render.hpp"
#include <glm/gtc/matrix_transform.hpp>

const char* vs_box = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec3 a_color;

out vec3 v_color;

uniform mat4 u_vp;

void main() {
    gl_Position = u_vp * vec4(a_pos, 0.0, 1.0);
    v_color = a_color;
})###";

const char* fs_box = R"###(
#version 330 core
in vec3 v_color;

out vec4 fragment_color;

void main() {
    fragment_color = vec4(v_color.xyz, 1.0);
})###";

namespace spk {
    void ui_system_on_add(flecs::entity e, ui_system_ctx_t& ctx) {

    }

    void ui_system_on_remove(flecs::entity e, ui_system_ctx_t& ctx) {

    }

    void update_ui_element(flecs::iter& iter, ui_comp_t* ui_comp, 
        flecs::entity root, glm::vec2 center, glm::vec2 size) {
        root.children([&](flecs::entity e) {
            const ui_comp_attribute_position_t* pos = e.get<ui_comp_attribute_position_t>();
            const ui_comp_attribute_text_t* text = e.get<ui_comp_attribute_text_t>();
            const ui_comp_attribute_size_t* box_size = e.get<ui_comp_attribute_size_t>();

            size_t i__ = 0;
            for(auto i : iter) {
                if(iter.entity(i) == e) {
                    i__ = i;
                    break;
                }
            }

            ui_comp_t* comp = &ui_comp[i__];
            glm::vec2 abs_pos = {0.0f, 0.0f};
            glm::vec2 abs_size = {0.0f, 0.0f};

            if(box_size) {
                if(box_size->absolute) {
                    abs_size = box_size->size;
                } else {
                    abs_size = determine_position(size, box_size->size);
                }    
            }

            if(pos) {
                if(pos->absolute) {
                    abs_pos = pos->position;
                } else {
                    abs_pos = determine_position(size, pos->position);
                }   
            }
                
            if(comp->centered) {
                abs_pos.x -= abs_size.x / 2;
                abs_pos.y -= abs_size.y / 2;
            }

            abs_pos = keep_inside_box(size, abs_size, abs_pos);

            comp->_cache.size = abs_size;
            comp->_cache.position = abs_pos;
            comp->margin = {0.0f, 0.0f};
            comp->padding = {0.0f, 0.0f};
            comp->_cache.update = false;
        });

        //NOTE: a quad tree would be better here

        // iterate through every entity that is child of root
        // if they are colliding, resolve the collision with respect
        // to margin, padding, etc.

        // root.children([&](flecs::entity e1) {
        //     ui_comp_t::cache_t* cache1 = &e1.get_mut<ui_comp_t>()->_cache;
        //     glm::vec2 e1box[2] = {
        //         { cache1->position.x, cache1->position.x + cache1->size.x },
        //         { cache1->position.y, cache1->position.y + cache1->size.y }
        //     };

        //     root.children([&](flecs::entity e2) {
        //         ui_comp_t::cache_t* cache2 = &e2.get_mut<ui_comp_t>()->_cache;
                
        //         glm::vec2 e2box[2] = {
        //             { cache2->position.x, cache2->position.x + cache2->size.x },
        //             { cache2->position.y, cache2->position.y + cache2->size.y }
        //         };

        //         if(e1 == e2)
        //             return;

        //         // collision detected so we have to resolve it
        //         //NOTE: this isn't like physics collision resolution so we can do much cheaper resolving steps
        //         if(vec2_aabb(e1box, e2box)) {
        //             // we can stack the element horizontally or vertically

        //             // this is vertical stacking
        //             cache1->position.x = cache2->position.x;
        //             cache1->position.y = cache2->position.y + cache2->size.y;    
        //         }                
        //     });
        // });
    }

    void ui_system_tick(flecs::iter& iter, ui_comp_t* ui_comp) {
        auto ctx = SPK_GET_CTX_REF(iter, ui_system_ctx_t);
        comp_window_t* window = state._get_current_window().get_mut<comp_window_t>();
        glm::vec2 size = static_cast<glm::vec2>(window->get_size());
        glm::vec2 center = { size.x / 2, size.y / 2 };
        
        update_ui_element(iter, ui_comp, state._get_current_canvas(), center, size);
    }

    void ui_render_system_on_add(flecs::entity e, ui_render_system_ctx_t& ctx) {
        uint32_t vs_shader, fs_shader;

        ctx.vertex_array.init();

        ctx.vertex_buffer.init(GL_ARRAY_BUFFER);
        ctx.vertex_buffer.buffer_data(sizeof(float) * 2 * 3 * 6 * 100, nullptr, GL_DYNAMIC_DRAW);
        ctx.vertex_layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0, ctx.vertex_buffer);
        ctx.vertex_layout.add(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 2, ctx.vertex_buffer);
        ctx.vertex_array.bind_layout(ctx.vertex_layout);

        vs_shader = sfk::create_shader_from_src(GL_VERTEX_SHADER, vs_box, nullptr);
        fs_shader = sfk::create_shader_from_src(GL_FRAGMENT_SHADER, fs_box, nullptr);
        
        sfk_assert(vs_shader != UINT32_MAX);
        sfk_assert(fs_shader != UINT32_MAX);
        
        ctx.program.init();
        DEBUG_VALUE(bool, ret =) ctx.program.load_shader_modules(vs_shader, fs_shader);
        sfk_assert(ret);
    
        ctx.mesh.resize(100 * 3);
    }

    void ui_render_system_on_remove(flecs::entity e, ui_render_system_ctx_t& ctx) {
        ctx.vertex_buffer.free();
        ctx.vertex_array.free();
        ctx.program.free();
    }

    void ui_render_system_resize(flecs::iter& iter) {
        auto ctx = SPK_GET_CTX_REF(iter, ui_render_system_ctx_t);
        comp_window_size_t* resize = iter.param<comp_window_size_t>();
        glm::mat4 view, proj;

        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        proj = glm::ortho(0.0f, (float)resize->width, (float)resize->height, 0.0f);
        ctx->vp = proj * view;
    }

    void ui_render_system_update(flecs::iter& iter, ui_comp_t* comp) {
        auto render_ctx = state._get_current_renderer().get_ref<render_system_ctx_t>();
        auto ui_ctx = SPK_GET_CTX_REF(iter, ui_render_system_ctx_t);
        const uint32_t quad_vert_count = 4;
        const uint32_t quad_index_count = 6;

        for(auto i : iter) {
            flecs::entity e = iter.entity(i);

            ui_ctx->mesh[0].pos = { comp->_cache.position.x,                       comp->_cache.position.y };
            ui_ctx->mesh[1].pos = { comp->_cache.position.x + comp->_cache.size.x, comp->_cache.position.y };
            ui_ctx->mesh[2].pos = { comp->_cache.position.x + comp->_cache.size.x, comp->_cache.position.y + comp->_cache.size.y };
            ui_ctx->mesh[3].pos = { comp->_cache.position.x,                       comp->_cache.position.y + comp->_cache.size.y };

            for(uint32_t j = 0; j < quad_vert_count; j++)  {
                ui_ctx->mesh[j].color = comp->color;
            }

            ui_ctx->vertex_buffer.buffer_sub_data(0,
                quad_vert_count * sizeof(ui_render_system_ctx_t::vertex_t), ui_ctx->mesh.data());
            ui_ctx->vertex_array.bind();

            render_ctx->quad_index_buffer.bind();
            ui_ctx->program.use();
            ui_ctx->program.set_mat4("u_vp", ui_ctx->vp);
            glDrawElements(GL_TRIANGLES, quad_index_count, GL_UNSIGNED_INT, nullptr);   
        }
    }   

    void ui_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        ui_comp_canvas_init(world);

        world.component<ui_system_ctx_t>();
        world.component<ui_render_system_ctx_t>();
        world.observer<ui_system_ctx_t>().event(flecs::OnAdd).each(ui_system_on_add);
        world.observer<ui_system_ctx_t>().event(flecs::OnRemove).each(ui_system_on_add);
        world.observer<ui_render_system_ctx_t>().event(flecs::OnAdd).each(ui_render_system_on_add);
        world.observer<ui_render_system_ctx_t>().event(flecs::OnRemove).each(ui_render_system_on_remove);

        auto ui_ctx = ctx_alloc.allocate_ctx<ui_system_ctx_t>();
        auto ui_render_ctx = ctx_alloc.allocate_ctx<ui_render_system_ctx_t>();

        world.system<ui_comp_t>()
            .ctx(ui_ctx).interval(state._get_target_tps()).iter(ui_system_tick);

        world.system<ui_comp_t>().kind(flecs::OnUpdate)
            .ctx(ui_render_ctx).iter(ui_render_system_update);
        
        world.observer().event<comp_window_size_t>().term<tag_events_t>()
            .ctx(ui_render_ctx).iter(ui_render_system_resize);
    }
}