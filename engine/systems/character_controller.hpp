#pragma once

#include "components/character_controller.hpp"

namespace spk {
    struct mesh_t;

    // character controller cs init will be called in sprite cs init
    void character_controller_cs_init(flecs::world& world);
}