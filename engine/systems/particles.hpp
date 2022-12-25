#pragma once

#include "sprite_render.hpp"
#include "../components/particle_system.hpp"

namespace spk {
    // particles_cs_init will be called by sprite_cs_init
    void _particles_cs_init(flecs::entity* ctx, flecs::world& world);    
}