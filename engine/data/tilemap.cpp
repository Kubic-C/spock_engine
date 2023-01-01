#include "tilemap.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void tilemap_t::init() {
        size.x = 100;
        size.y = 100;

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
    
    void tilemap_t::iterate_map(std::function<void(uint32_t x, uint32_t y)>&& clbk) {
        for(uint32_t x = 0; x < size.x; x++) {
            for(uint32_t y = 0; y < size.y; y++) {
                clbk(x, y);
            }
        }
    }

    void tilemap_t::iterate_colliadable(std::function<void(uint32_t x, uint32_t y, tile_is_coll_info_t&)>&& clbk) {
        iterate_map([&](uint32_t x, uint32_t y) {
            tile_is_coll_info_t tile_info = tile_is_colliadable(x, y);
            
            if(tile_info.is_colliadable()) {
                clbk(x, y, tile_info);
            }
         });
    }
    
    void tilemap_t::iterate_non_zero(std::function<void(uint32_t x, uint32_t y)>&& clbk) {
        iterate_map([&](uint32_t x, uint32_t y) {
            if(!is_tile_empty(tiles[x][y])) {
                clbk(x, y);
            }
        });
    }

    tile_is_coll_info_t tilemap_t::tile_is_colliadable(uint32_t x, uint32_t y) {
        tile_is_coll_info_t info;

        if(spk::is_tile_empty(tiles[x][y])) // if its empty it can't collide
            return info;

        // check left tile
        if(x - 1 != UINT32_MAX) {
            if(spk::is_tile_empty(tiles[x - 1][y])) {
                info.left = true;
            }
        } else { // edge tiles are colliadable
            info.left = true;
        }

        // check right tile
        if(x + 1 != size.x) {
            if(spk::is_tile_empty(tiles[x + 1][y])) {
                info.right = true;
            }
        } else { // edge tiles are colliadable
            info.right = true;
        }

        // check top tiles
        if(y - 1 != UINT32_MAX) {
            if(spk::is_tile_empty(tiles[x][y - 1])) {
                info.top = true;
            }
        } else { // edge tiles are colliadable
            info.top = true;
        }

        // check bottom tiles
        if(y + 1 != size.y) {
            if(spk::is_tile_empty(tiles[x][y + 1])) {
                info.bottom = true;
            }
        } else { // edge tiles are colliadable
            info.bottom = true;
        }

        return info;
    }

    
    void tilemap_t::compute_colliders() {
        float half_width = (float)size.x;
        float half_height = (float)size.y / 2.0f;

        compute_centroid();
        colliding_tiles.clear();

        iterate_colliadable([&](uint32_t x, uint32_t y, tile_is_coll_info_t& info){
            colliding_tiles.push_back((tile_collider_t){.id = tiles[x][y].id});
            auto& shape = colliding_tiles.back().shape;

            shape.SetAsBox(SPK_TILE_HALF_SIZE, SPK_TILE_HALF_SIZE);
            shape.m_vertices[0] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
            shape.m_vertices[1] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
            shape.m_vertices[2] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
            shape.m_vertices[3] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
        });

        iterate_non_zero([&](uint32_t x, uint32_t y) {
            tile_t& tile = tiles[x][y];
            tile_metadata_t& md = state.engine->rsrc_mng.get_tile_dictionary()[tile.id];
            
            // Compute the mass for a given tile
            const float area = SPK_TILE_HALF_SIZE * SPK_TILE_HALF_SIZE; // in sqaure meters
            mass += area * md.density;
        });
    }


    void tilemap_t::compute_centroid() {
        float left_most   = std::numeric_limits<float>().max(), 
              right_most  = 0, 
              top_most    = 0, 
              bottom_most = std::numeric_limits<float>().max();

        iterate_colliadable([&](uint32_t x, uint32_t y, tile_is_coll_info_t&){
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

        // adding by the half distance to find the center
        center.x = left_most   + (right_most - left_most) / 2.0f; 
        center.y = bottom_most + (top_most - bottom_most) / 2.0f; 
    }
} 