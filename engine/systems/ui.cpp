/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "ui.hpp"
#include "utility/ui.hpp"
#include "window.hpp"
#include "core/internal.hpp"

namespace spk {
    void dimensions_update(ptr_t<container_t> container) {
        container->dimensions_calculate();

        for(auto child : container->children) {
            dimensions_update(child);
        }
    }

    void ui_render_system_pre_update(flecs::iter& iter) {
        dimensions_update(ptr_t(&canvas()));
    }

    void ui_render_system_resize(flecs::iter& iter) {
        event_window_size_t* resize = iter.param<event_window_size_t>();
        
    }

    void ui_system_mouse_callback(flecs::iter& iter) {

    }

    void ui_cs_init(flecs::world& world) {
        spk_trace();

        world.system().kind(on_pre_update_id).iter(ui_render_system_pre_update);
        
        world.observer().event<event_window_size_t>().term<tag_events_t>()
            .iter(ui_render_system_resize);

        world.observer().event<event_window_mouse_click_t>().term<tag_events_t>()
            .iter(ui_system_mouse_callback);
    }
}