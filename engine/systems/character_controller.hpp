#pragma once

#include "components/character_controller.hpp"

namespace spk {
    struct mesh_t;

    // character controller cs init will be called in sprite cs init
    void _character_controller_cs_init(mesh_t* mesh, flecs::world& world);
}