#pragma once

#include "box2D.hpp"
#include "../data/atlas.hpp"

namespace spk {
    struct comp_sprite_t {
        void init() {}
        void free() {}

        uint32_t atlas_id;

        // sprite's position on the in x and y
        uint32_t tax, tay;
        glm::vec2 size;
    };

    void sprite_comp_init(flecs::world& world);
}