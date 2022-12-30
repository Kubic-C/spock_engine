#pragma once

#include "components/tilebody.hpp"

namespace spk {
    struct sprite_batch_mesh_t;

    void _tilebody_cs_init(sprite_batch_mesh_t* ctx, flecs::world& world);
}