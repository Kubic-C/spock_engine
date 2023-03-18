/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "components/rigid_body.hpp"
#include "physics/collision_listener.hpp"

namespace spk {
    void physics_cs_init(flecs::world& world);
}