#pragma once

#include "components/tilemap.hpp"

namespace spk {
    struct mesh_t;

    // tilemap cs init will be called by sprite cs init
    void tilemap_cs_init(flecs::world& world);
}