/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "components/tilemap.hpp"

namespace spk {
    struct mesh_t;

    // tilemap cs init will be called by sprite cs init
    void tilemap_cs_init(flecs::world& world);
}