#include "render.hpp"
#include "state.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    void render_system_resize(flecs::iter& iter) {
        auto                 rs     = state.get_current_renderer();
        event_window_size_t* resize = iter.param<event_window_size_t>();

       rs->resize(resize->width, resize->height); 
    }

    void render_system_begin_render(flecs::iter& iter, comp_window_t* window) {
        spk_assert(iter.count() == 1);

        auto rs = state.get_current_renderer();

        rs->begin_frame();
    }

    void render_system_render(flecs::iter& iter, comp_window_t* window) {
        spk_assert(iter.count() == 1);

        auto rs = state.get_current_renderer();

        rs->draw_frame();
    }

    void render_system_end_render(flecs::iter& iter, comp_window_t* window) {
        spk_assert(iter.count() == 1);

        auto rs = state.get_current_renderer();

        rs->end_frame(window->win);
    }

    void render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        render_system_t* ctx = ctx_alloc.allocate_ctx<render_system_t>();

        state.set_current_renderer(ctx);

        world.system<comp_window_t>().term<tag_current_window_t>() // PRE
            .kind(on_render_begin).iter(render_system_begin_render).add<tag_render_system_t>();
        world.system<comp_window_t>().term<tag_current_window_t>() // ON
            .kind(on_render).iter(render_system_render).add<tag_render_system_t>(); 
        world.system<comp_window_t>().term<tag_current_window_t>() // POST
            .kind(on_render_end).iter(render_system_end_render).add<tag_render_system_t>();

        world.observer().event<event_window_size_t>().term<tag_events_t>().iter(render_system_resize);
    }
}