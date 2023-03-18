#include "ui.hpp"
#include "utility/ui.hpp"
#include "window.hpp"
#include "core/internal.hpp"

namespace spk {
    void ui_render_system_update(flecs::iter& iter) {

    }

    void ui_render_system_resize(flecs::iter& iter) {
        event_window_size_t* resize = iter.param<event_window_size_t>();
        
    }

    void ui_system_mouse_callback(flecs::iter& iter) {

    }

    void ui_cs_init(flecs::world& world) {
        spk_trace();
        
        world.observer().event<event_window_size_t>().term<tag_events_t>()
            .iter(ui_render_system_resize);

        world.observer().event<event_window_mouse_click_t>().term<tag_events_t>()
            .iter(ui_system_mouse_callback);
    }
}