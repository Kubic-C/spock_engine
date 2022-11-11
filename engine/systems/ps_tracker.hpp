#pragma once 

#include "../systems.hpp"

// per second tracker | UPS TPS DELTA-TIME

namespace spk {
    struct ps_tracker_ctx_t {
        uint32_t fps = 0;
        uint32_t tps = 0;
    };

    void ps_tracker_system_tick(ps_tracker_ctx_t& ctx);
    void ps_tracker_system_update(ps_tracker_ctx_t& ctx);
    void ps_tracker_system_update_sec(flecs::iter& iter, ps_tracker_ctx_t* ctx);
    void ps_tracker_system_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world);
}