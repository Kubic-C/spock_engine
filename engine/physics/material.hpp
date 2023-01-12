#pragma once 

#include "base.hpp"

namespace spk {
    struct material_t {
        float density     = 1.0f;
        float restitution = 0.5f;
        float friction    = 0.2f;
    };
}