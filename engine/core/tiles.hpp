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

namespace spk {
    enum tile_flags_t {
        TILE_FLAGS_COLLIADABLE = 1 << 0
    };

    struct tile_metadata_t {
        tile_metadata_t() {
            sprite.size = {SPK_TILE_HALF_SIZE, SPK_TILE_HALF_SIZE};
        }

        sprite_arrayd_t sprite;
        
        float density     = 1.0f;
        float friction    = 0.2f;
		float restitution = 0.0f;
    };

    struct tile_t {
        uint32_t id    = 0;
        uint8_t  flags = TILE_FLAGS_COLLIADABLE;
        float    hp    = 100.0f;  

        tile_t() {
        }

        tile_t(uint32_t id)
            : id(id) {
            
        }
    };

    typedef std::unordered_map<uint32_t, tile_metadata_t> tile_dictionary_t;

    inline bool is_tile_empty(const tile_t& tile) {
        return (tile.id == 0 ||  !(tile.flags & TILE_FLAGS_COLLIADABLE));
    }
}