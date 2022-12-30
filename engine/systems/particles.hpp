#pragma once

#include "components/particles.hpp"

namespace spk {
    struct sprite_batch_mesh_t;

    // particles_cs_init will be called by sprite_cs_init
    void _particles_cs_init(sprite_batch_mesh_t* ctx, flecs::world& world);    
}