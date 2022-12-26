#include "tilemap.hpp"


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
                tiles[x][y].id = 0;
                tiles[x][y].flags = TILE_FLAGS_COLLIADABLE;
            }
        }
    }

    void tilemap_t::free() {

    }

    void tilemap_t::iterate_colliadable(std::function<void(uint32_t x, uint32_t y)>&& clbk) {
        for(uint32_t x = 0; x < size.x; x++) {
            for(uint32_t y = 0; y < size.y; y++) {
                if(tile_is_colliadable(x, y)) {
                    clbk(x, y);
                }
            }
        }
    }

    bool tilemap_t::tile_is_colliadable(uint32_t x, uint32_t y) {
        if(spk::is_tile_empty(tiles[x][y])) // if its empty it can't collide
            return false;

        // check left tile
        if(x - 1 != UINT32_MAX) {
            if(spk::is_tile_empty(tiles[x - 1][y])) {
                return true;
            }
        } else { // edge tiles are colliadable
            return true;
        }

        // check right tile
        if(x + 1 != size.x) {
            if(spk::is_tile_empty(tiles[x + 1][y])) {
                return true;
            }
        } else { // edge tiles are colliadable
            return true;
        }

        // check top tiles
        if(y - 1 != UINT32_MAX) {
            if(spk::is_tile_empty(tiles[x][y - 1])) {
                return true;
            }
        } else { // edge tiles are colliadable
            return true;
        }

        // check bottom tiles
        if(y + 1 != size.y) {
            if(spk::is_tile_empty(tiles[x][y + 1])) {
                return true;
            }
        } else { // edge tiles are colliadable
            return true;
        }

        return false;
    }

    void tilemap_t::find_colliding_tiles() {
        float half_width = (float)size.x;
        float half_height = (float)size.y / 2.0f;

        find_center();
        colliding_tiles.clear();

        iterate_colliadable([&](uint32_t x, uint32_t y){
            colliding_tiles.push_back((tile_collider_t){.index = {x, y}});
            auto& shape = colliding_tiles.back().shape;

            shape.SetAsBox(SPK_TILE_HALF_SIZE, SPK_TILE_HALF_SIZE);
            shape.m_vertices[0] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
            shape.m_vertices[1] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
            shape.m_vertices[2] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
            shape.m_vertices[3] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
        });
    }

    void tilemap_t::find_center() {
        uint32_t left_most   = UINT32_MAX, 
                 right_most  = 0, 
                 top_most    = 0, 
                 bottom_most = UINT32_MAX;

        iterate_colliadable([&](uint32_t x, uint32_t y){
            if(x < left_most) { // x is more left
                left_most = x;
            } else if(x > right_most) { // x is more right
                right_most = x;
            }

            if(y > top_most) { // y is higher
                top_most = y;
            } else if(y < bottom_most) { // y is less
                bottom_most = y;
            }
        });

        // dividing by the half distance to find the center
        center.x = left_most   + (float)(right_most - left_most) / 2.0f; 
        center.y = bottom_most + (float)(top_most - bottom_most) / 2.0f; 
    }
} 