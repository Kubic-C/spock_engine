#pragma once

#include "components/components.hpp"
#include "systems.hpp"

namespace spk {
    // component Component System initialization
    void component_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world);
}