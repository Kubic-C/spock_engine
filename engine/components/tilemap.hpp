#pragma once

#include "utility/array2D.hpp"
#include "data/tiles.hpp"
#include "debug.hpp"
#include "box2D.hpp"

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

    struct comp_tilemap_t {
        struct tile_collider_t {
            b2PolygonShape shape;
            uint32_t id;
        };

        float mass = 0.0f;
        glm::vec2 center; // gives it an exact center, not a tile center

        glm::vec2 offset; // its local offset to its tilebody's, or body's world position
        glm::uvec2 size;
        array2D_t<tile_t> tiles;

        // tiles that should have an active collider on them
        std::vector<tile_collider_t> colliding_tiles;

        void init();
        void free();

        void add_fixtures(b2Body* body);
        void iterate_map(std::function<void(uint32_t x, uint32_t y)>&& clbk);
        void iterate_non_zero(std::function<void(uint32_t x, uint32_t y)>&& clbk);
        void iterate_colliadable(std::function<void(uint32_t x, uint32_t y, tile_is_coll_info_t&)>&& clbk);
        tile_is_coll_info_t tile_is_colliadable(uint32_t x, uint32_t y);
        void compute_colliders();
        void compute_centroid();
    };

    void tile_comp_init(flecs::world& world);
}