#pragma once

#include "tiles.hpp"

namespace spk {
    struct tilemap_t {
        struct tile_collider_t {
            b2PolygonShape shape;
            glm::uvec2 index;
        };

        // a tilemap is composed of two things.
        // its rendering mesh;
        // and its physics mesh

        glm::vec2 offset; // its local offset to its tilebody's, or body's world position
        glm::uvec2 size;
        std::vector<std::vector<tile_t>> tiles;

        // tiles that should have an active collider on them
        std::vector<tile_collider_t> colliding_tiles;

        void init();
        void free();

        bool tile_is_colliadable(uint32_t x, uint32_t y);
        void find_colliding_tiles();
    };
}