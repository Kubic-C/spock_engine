#include "tilemap.hpp"

#define TILE_EMPTY(tile) (tile.flags & TILE_FLAGS_EMPTY)

namespace spk {
    void tilemap_t::init() {
        size.x = 10;
        size.y = 10;

        tiles.resize(size.x);
        for(uint32_t i = 0; i < size.y; i++) {
            tiles[i].resize(size.y);
        }

        for(uint32_t x = 0; x < size.x; x++) {
            for(uint32_t y = 0; y < size.y; y++) {
                tiles[x][y].flags |= TILE_FLAGS_EMPTY;
            }
        }
    }

    void tilemap_t::free() {

    }

    bool tilemap_t::tile_is_colliadable(uint32_t x, uint32_t y) {
        if(TILE_EMPTY(tiles[x][y])) // if its empty it can't collide
            return false;

        // check left tile
        if(x - 1 != UINT32_MAX) {
            if(TILE_EMPTY(tiles[x - 1][y])) {
                return true;
            }
        } else { // edge tiles are colliadable
            return true;
        }

        // check right tile
        if(x + 1 != size.x) {
            if(TILE_EMPTY(tiles[x + 1][y])) {
                return true;
            }
        } else { // edge tiles are colliadable
            return true;
        }

        // check top tiles
        if(y - 1 != UINT32_MAX) {
            if(TILE_EMPTY(tiles[x][y - 1])) {
                return true;
            }
        } else { // edge tiles are colliadable
            return true;
        }

        // check bottom tiles
        if(y + 1 != size.y) {
            if(TILE_EMPTY(tiles[x][y + 1])) {
                return true;
            }
        } else { // edge tiles are colliadable
            return true;
        }

        return false;
    }

    void tilemap_t::find_colliding_tiles() {
        float half_width = (float)size.x / 2.0f;
        float half_height = (float)size.y / 2.0f;

        colliding_tiles.clear();

        for(uint32_t x = 0; x < size.x; x++) {
            for(uint32_t y = 0; y < size.y; y++) {
                bool empty_near;
                
                if(tiles[x][y].flags & TILE_FLAGS_COLLIADABLE)
                    goto add_collide;

                empty_near = tile_is_colliadable(x, y);
                
                if(empty_near) {
                add_collide:
                    colliding_tiles.push_back((tile_collider_t){.index = {x, y}});
                    auto& shape = colliding_tiles.back().shape;

                    shape.SetAsBox(SPK_TILE_HALF_SIZE, SPK_TILE_HALF_SIZE);
                    shape.m_vertices[0] += (b2Vec2){(float)x, (float)y};
                    shape.m_vertices[1] += (b2Vec2){(float)x, (float)y};
                    shape.m_vertices[2] += (b2Vec2){(float)x, (float)y};
                    shape.m_vertices[3] += (b2Vec2){(float)x, (float)y};
                }
            }
        }

    }
}