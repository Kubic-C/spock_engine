#pragma once

#include "framework.hpp"

#define TILE_PER_CHUNK_SIDE 16
#define CHUNK_PER_BODY_SIDE 8
#define TILE_PER_CHUNK (TILE_PER_CHUNK_SIDE * TILE_PER_CHUNK_SIDE)
#define CHUNK_PER_BODY (CHUNK_PER_BODY_SIDE * CHUNK_PER_BODY_SIDE)

namespace spk {
    enum {
        CHUNK_FLAGS_EMPTY = 1
    } chunk_flags_e;

    struct tile_vertex_t {
        glm::vec2 pos;
        glm::vec2 uv;
    };

    struct tile_t {
        sfk::key_t key;
    };

    struct chunk_t {
        tile_t tiles[TILE_PER_CHUNK];

        bool update;
        uint32_t tile_count;
        uint32_t flags;
        b2Fixture* fixture;
    };
    
    struct tilemap_t {
        chunk_t* chunk;
        b2Body* body;

        void init(b2World* world);
        void set_tile(uint32_t x, uint32_t y, sfk::key_t key);
        void tick();
        void free();
    };
}