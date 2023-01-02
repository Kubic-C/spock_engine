#pragma once

#include "box2D.hpp"
#include "data/atlas.hpp"

namespace spk {
    struct comp_sprite_t {
        void init() { atlas_id = 0; tax = 0; tay = 0; size = {0.5f, 0.5f}; z = 0.0f; }
        void free() {}

        uint32_t  atlas_id; // the sprite atlas to get sprite data from
        uint32_t  tax, tay; // texture coord offsets in the sprite atlas
        glm::vec2 size;     // the size of the sprite to render
        float     z;        // z coordinate, allows overlaying sprites overeachother in a reliable way

        b2Fixture* add_fixture(b2Body* body, float density, float friction, float restitution);
    };

    void sprite_comp_init(flecs::world& world);
}