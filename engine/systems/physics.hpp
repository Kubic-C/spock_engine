#pragma once

#include "../systems.hpp"
#include "../components/box2D.hpp"

namespace spk {
    
    void physics_system_tick(flecs::entity e, comp_b2World_t& world, tag_current_box2d_world_t);
    void physics_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world);
}