#pragma once 

#include "systems.hpp"
#include "components/window.hpp"
#include "systems.hpp"


namespace spk {
    struct window_system_ctx_t : public system_t {
        SDL_Event event;
    };

    void window_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world);
}
