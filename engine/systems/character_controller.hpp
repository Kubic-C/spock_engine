#pragma once

#include "components/character_controller.hpp"

namespace spk {
    struct sprite_batch_mesh_t;

    void _character_controller_cs_init(sprite_batch_mesh_t* mesh, flecs::world& world);
}