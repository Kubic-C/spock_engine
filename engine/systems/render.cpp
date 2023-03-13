#include "render.hpp"
#include "core/internal.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    void render_system_resize(flecs::iter& iter) {
        spk_trace();
        
        auto                 rs     = internal->scene.renderer;
        event_window_size_t* resize = iter.param<event_window_size_t>();

        rs->resize(resize->width, resize->height); 
    }

    void render_system_begin_render(flecs::iter& iter) {
        spk_trace();
        spk_assert(iter.count() == 0);

        auto rs = internal->scene.renderer;

        rs->begin_frame();
    }

    void render_system_render(flecs::iter& iter) {
        spk_trace();
        spk_assert(iter.count() == 0);

        auto rs = internal->scene.renderer;

        rs->draw_frame();
    }

    void render_system_end_render(flecs::iter& iter) {
        spk_trace();
        spk_assert(iter.count() == 0);

        auto rs = internal->scene.renderer;

        rs->end_frame(internal->scene.window->get_sdl_window());
    }

    void render_cs_init(flecs::world& world) {
        spk_trace();

        render_system_t* renderer = internal->allocators.stack.push<render_system_t>();

        internal->scene.renderer = renderer;

        world.system().kind(on_render_begin).iter(render_system_begin_render).add<tag_render_system_t>();
        world.system().kind(on_render).iter(render_system_render).add<tag_render_system_t>(); 
        world.system().kind(on_render_end).iter(render_system_end_render).add<tag_render_system_t>();

        world.observer().event<event_window_size_t>().term<tag_events_t>().iter(render_system_resize);
    }
}