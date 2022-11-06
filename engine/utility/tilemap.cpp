#include "tilemap.hpp"

namespace spk {
    void tilemap_t::init(b2World* world) {
        chunk = new chunk_t[CHUNK_PER_BODY];

        b2BodyDef def;
        body = world->CreateBody(&def);

        for(uint32_t i; i < CHUNK_PER_BODY; i++) {
            chunk[i].flags = 0;
            chunk[i].update = true;
        }
    }

    void tilemap_t::set_tile(uint32_t x, uint32_t y, sfk::key_t key) {
        uint32_t lx = x % TILE_PER_CHUNK_SIDE;
        uint32_t ly = y % TILE_PER_CHUNK_SIDE;
        uint32_t cx = (x - lx) / TILE_PER_CHUNK_SIDE;
        uint32_t cy = (y - ly) / TILE_PER_CHUNK_SIDE;
        uint32_t ci = cy * CHUNK_PER_BODY_SIDE + cx;
        uint32_t ti = ly * TILE_PER_CHUNK_SIDE + lx;

        assert(chunk[ci].tiles[ti].key != key); // abusrd

        chunk[ci].tiles[ti].key = key;
        chunk[ci].update = true;
    }

    void tilemap_t::free() {
        delete chunk;
    }
}