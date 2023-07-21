/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "components/sprite.hpp"

// in meters
#define SPK_TILE_HALF_SIZE 0.5f
#define SPK_TILE_FULL_SIZE (SPK_TILE_HALF_SIZE + SPK_TILE_HALF_SIZE) 

namespace spk {
    enum tile_flags_t {
        TILE_FLAGS_COLLIADABLE = 1 << 0
    };

    struct tile_sprite_t {
        uint32_t id    = 0;
        uint32_t index = 0;
        float    z     = 0.0f;
    };

    struct tile_t {
        tile_t() {}
                
        bool empty() { return !fixture && sprite.id == 0; }
        bool is_renderable() { return sprite.id != 0; }

        float           density     = 1.0f;
        float           friction    = 0.2f;
		float           restitution = 0.0f;
        kin::fixture_t* fixture     = nullptr;

        comp_animate_t animation;
        tile_sprite_t  sprite;

        uint8_t        flags      = 0;
    };
}