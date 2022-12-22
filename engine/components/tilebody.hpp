#pragma once

#include "../data/tilemap.hpp"

#define spk_register_game_component(world, type) \
        world.component<type>() \
            .on_add([](type& comp){ DEBUG_EXPR(sfk::log.log("on add hook | " #type)); comp.init(); }) \
            .on_set([](type& comp){ DEBUG_EXPR(sfk::log.log("on set hook | " #type)); }) \
            .on_remove([](type& comp){ DEBUG_EXPR(sfk::log.log("on remove hook | " #type)); comp.free();})

namespace spk {
    struct comp_tilebody_t {
        tilemap_t tilemap;
        tile_dictionary_t dictionary;
        b2Body* body;

        void init();
        void free();

        void add_fixtures();
    };
    
    void tile_comp_init(flecs::world& world);
}