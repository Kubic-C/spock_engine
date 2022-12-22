#pragma once

#include "../components/sprite.hpp"

// in meters
#define SPK_TILE_HALF_SIZE 0.5f

namespace spk {
    enum tile_flags_t {
        TILE_FLAGS_COLLIADABLE = 1 << 0,
        TILE_FLAGS_EMPTY       = 1 << 1
    };

    struct tile_metadata_t {
        comp_sprite_t sprite = { .size = {SPK_TILE_HALF_SIZE, SPK_TILE_HALF_SIZE}, };
        uint8_t default_flags = TILE_FLAGS_COLLIADABLE;
        float density     = 1.0f;
        float friction    = 0.2f;
		float restitution = 0.0f;
    };

    struct tile_t {
        uint32_t id   = 0;
        uint8_t flags = 0;
    };

    typedef std::unordered_map<uint32_t, tile_metadata_t> tile_dictionary_t;
}