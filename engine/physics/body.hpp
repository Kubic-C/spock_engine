#pragma once

#include "transform.hpp"
#include "material.hpp"
#include "aabb.hpp"

namespace spk {
    struct fixture_t {
        material_t material;
        uint32_t   type;
    };

    struct rigid_body_t {
        transform_t transform;
        glm::vec2   velocity;
        float       angular_velocity;
    };
}