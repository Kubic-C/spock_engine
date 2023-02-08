#pragma once

#include "systems.hpp"
#include "physics/world.hpp"

namespace spk {
    void physics_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world);
}