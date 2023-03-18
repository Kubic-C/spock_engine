/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "components/particles.hpp"

namespace spk {
    struct mesh_t;

    void particles_cs_init(flecs::world& world);    
}