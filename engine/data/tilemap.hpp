#pragma once

#include "tiles.hpp"

#define SPK_TILE_EMPTY(tile) (tile.id == 0 ||  !(tiles[x][y].flags & TILE_FLAGS_COLLIADABLE))

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
        glm::vec2 center; // gives it an exact center, not a tile center
        std::vector<std::vector<tile_t>> tiles;

        // tiles that should have an active collider on them
        std::vector<tile_collider_t> colliding_tiles;

        void init();
        void free();

        void iterate_colliadable(std::function<void(uint32_t x, uint32_t y)>&& clbk);
        bool tile_is_colliadable(uint32_t x, uint32_t y);
        void find_colliding_tiles();
        void find_center();
    };
}