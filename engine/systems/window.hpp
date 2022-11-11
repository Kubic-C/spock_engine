#pragma once 

#include "../systems.hpp"
#include "../components/window.hpp"
#include "../systems.hpp"


namespace spk {
    struct window_system_ctx_t {
        SDL_Event event;
    };

    void window_system_update(flecs::iter& iter, comp_window_t* window);
    void window_system_current_update(comp_window_t& window, tag_current_window_t);
    void window_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world);
}
