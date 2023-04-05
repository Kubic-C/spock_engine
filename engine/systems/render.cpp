/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "render.hpp"
#include "core/internal.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    void render_system_resize(flecs::iter& iter) {
        spk_trace();

        event_window_size_t* resize = iter.param<event_window_size_t>();

        render_context().frame_resize(resize->width, resize->height); 
    }

    void render_system_begin_render(flecs::iter& iter) {
        spk_trace();
        spk_assert(iter.count() == 0);

        render_context().frame_begin();
    }

    void render_system_render(flecs::iter& iter) {
        spk_trace();
        spk_assert(iter.count() == 0);

        render_context().frame_render();
    }

    void render_system_end_render(flecs::iter& iter) {
        spk_trace();
        spk_assert(iter.count() == 0);

        render_context().frame_end();
    }

    void render_system_init(const flecs::world& world) {
        spk_trace();

        world.system().kind(on_render_begin_id).iter(render_system_begin_render).add<tag_renderer_t>();
        world.system().kind(on_render_id).iter(render_system_render).add<tag_renderer_t>(); 
        world.system().kind(on_render_end_id).iter(render_system_end_render).add<tag_renderer_t>();

        world.observer().event<event_window_size_t>().term<tag_events_t>().iter(render_system_resize);
    }
}