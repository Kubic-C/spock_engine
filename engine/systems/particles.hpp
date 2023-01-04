#pragma once

#include "components/particles.hpp"

namespace spk {
    struct mesh_t;

    // particles_cs_init will be called by sprite_cs_init
    void _particles_cs_init(mesh_t* ctx, flecs::world& world);    
}