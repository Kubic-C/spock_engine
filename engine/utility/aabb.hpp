#pragma once

#include "framework.hpp"

namespace spk {
    enum { min = 0, max = 1 }; 
    typedef glm::vec2 rect_tt[2]; 

    struct aabb_tt {
        float half_width;
        float half_height;

        aabb_tt() {
            half_width = 1.0f;
            half_height = 1.0f;
        }
    };
}