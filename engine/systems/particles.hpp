#pragma once

#include "components/particles.hpp"

namespace spk {
    // particles_cs_init will be called by sprite_cs_init
    void _particles_cs_init(flecs::entity* ctx, flecs::world& world);    
}