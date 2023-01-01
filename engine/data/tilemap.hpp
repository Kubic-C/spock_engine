#pragma once

#include "tiles.hpp"

#define SPK_TILE_EMPTY(tile) (tile.id == 0 ||  !(tiles[x][y].flags & TILE_FLAGS_COLLIADABLE))

namespace spk {
    struct tile_is_coll_info_t {
        bool left   = false;
        bool right  = false;
        bool top    = false;
        bool bottom = false;

        bool is_colliadable() {
            return left || right || top || bottom;
        }
    };

    struct tilemap_t {

        struct tile_collider_t {
            b2PolygonShape shape;
            uint32_t id;
        };

        float mass = 0.0f;
        glm::vec2 center; // gives it an exact center, not a tile center

        glm::vec2 offset; // its local offset to its tilebody's, or body's world position
        glm::uvec2 size;
        std::vector<std::vector<tile_t>> tiles;

        // tiles that should have an active collider on them
        std::vector<tile_collider_t> colliding_tiles;

        void init();
        void free();

        void iterate_map(std::function<void(uint32_t x, uint32_t y)>&& clbk);
        void iterate_non_zero(std::function<void(uint32_t x, uint32_t y)>&& clbk);
        void iterate_colliadable(std::function<void(uint32_t x, uint32_t y, tile_is_coll_info_t&)>&& clbk);
        tile_is_coll_info_t tile_is_colliadable(uint32_t x, uint32_t y);
        void compute_colliders();
        void compute_centroid();
    };
}