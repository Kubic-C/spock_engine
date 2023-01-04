#include "physics.hpp"
#include "state.hpp"

namespace spk {
    void physics_system_tick(flecs::entity e, comp_b2World_t& world, tag_current_box2d_world_t) {
        world.world->Step(state.get_target_tps(), 4, 2);
    }

    void physics_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        box2d_comp_init(world);

        world.system<comp_b2World_t, tag_current_box2d_world_t>().each(physics_system_tick);
    }
}