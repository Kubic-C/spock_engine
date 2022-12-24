#pragma once

#include "../data/tilemap.hpp"
#include "../debug.hpp"

namespace spk {
    struct comp_tilebody_t {
        tilemap_t tilemap;
        b2Body* body;

        void init();
        void free();

        void add_fixtures();
    };
    
    void tile_comp_init(flecs::world& world);
}