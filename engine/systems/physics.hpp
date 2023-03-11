#pragma once

#include "systems.hpp"
#include "components/rigid_body.hpp"
#include "physics/collision_listener.hpp"

namespace spk {
    void physics_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world);
}