#include "physics.hpp"
#include "../state.hpp"

namespace spk {
    void physics_system_tick(flecs::entity e, comp_box2d_world_t& world, tag_current_box2d_world_t) {
        world.world->Step(state._get_target_tps(), 8, 3);
    }

    void physics_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        comp_box2d_world_init(world);
        comp_b2Body_init(world);

        world.system<comp_box2d_world_t, tag_current_box2d_world_t>().interval(state._get_target_tps()).each(physics_system_tick);
    }
}