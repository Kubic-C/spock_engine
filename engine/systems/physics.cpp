#include "physics.hpp"
#include "state.hpp"
#include "physics/body.hpp"

namespace spk {
    void physics_system_tick(flecs::iter iter) {
        auto physics_world = state.get_current_physics_world();
    
        physics_world->step(stats.delta_time);
    }

    void physics_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        spk_trace();        

        auto ctx = ctx_alloc.allocate_ctx<physics_world_t>();

        world.system().iter(physics_system_tick);

        state.set_current_physics_world(ctx);
    }
}