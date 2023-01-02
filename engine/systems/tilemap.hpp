#pragma once

#include "components/tilemap.hpp"

namespace spk {
    struct sprite_batch_mesh_t;

    void _tilemap_cs_init(sprite_batch_mesh_t* ctx, flecs::world& world);
}