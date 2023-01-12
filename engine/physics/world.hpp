#pragma once

#include "body.hpp"
#include "utility/structures.hpp"

namespace spk {
    struct physics_world_t {
        memory_pool_t<rigid_body_t, 8> bodies;
    };
}