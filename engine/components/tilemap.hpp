#pragma once

#include "utility/array2D.hpp"
#include "data/tiles.hpp"
#include "debug.hpp"

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

    struct tile_group_t {
        uint32_t x = 1, y = 1;
    };

    struct comp_tilemap_t {
        float     width  = 0.0f;
        float     height = 0.0f;
        glm::vec2 center = {0.0f, 0.0f}; // gives it an exact center, not a tile center

        glm::vec2                        offset; // its local offset to its tilebody's, or body's world position
        array2D_t<tile_t>                tiles;
        std::unordered_map<uint32_t, tile_group_t> tile_groups; // 1D coords are used instead of 2D 

        void init(flecs::entity entity);
        void free(flecs::entity entity);
        void iterate_all(std::function<void(uint32_t x, uint32_t y)>&& clbk);
        void iterate_non_zero(std::function<void(uint32_t x, uint32_t y)>&& clbk);
        void iterate_colliadable(std::function<void(uint32_t x, uint32_t y, tile_is_coll_info_t&)>&& clbk); // tile is not surronded
        tile_is_coll_info_t tile_is_colliadable(uint32_t x, uint32_t y);
        
        void update_tilemap();
        // void compute_colliders();
        void compute_greedy_mesh();
        void compute_centroid();
    };

    void tile_comp_init(flecs::world& world);
}