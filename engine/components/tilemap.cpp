#include "tilemap.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void comp_tilemap_t::init(flecs::entity entity) {
        tiles.size(100, 100);

        for(uint32_t x = 0; x < tiles.get_width(); x++) {
            for(uint32_t y = 0; y < tiles.get_height(); y++) {
                tiles.get(x, y).id = 0;
                tiles.get(x, y).flags = TILE_FLAGS_COLLIADABLE;
            }
        }
    }

    void comp_tilemap_t::free(flecs::entity entity) {
    }
    
    void comp_tilemap_t::compute_greedy_mesh() {
        tile_groups.clear();

        // find tiles that have the same ID and adjacent on the Y-axis
        for(uint32_t x = 0; x < tiles.get_width(); x++) {
            for(uint32_t y = 0; y < tiles.get_height(); y++) {
                uint32_t last_index = tiles.get_1D_from_2D(x, y - 1);
                uint32_t cur_index = tiles.get_1D_from_2D(x, y);

                // We have to do this becuase,
                // if a tile has no adjacent tiles on the y axis
                // it will not be added to the map at all
                if(tile_groups.find(cur_index) == tile_groups.end())
                    tile_groups[cur_index] = { 1, 1 };

                // continue because, y - 1 (a few lines below) will be 
                // y - 1 [->] 0 - 1 [->] UINT32_MAX, which will surely cause 
                // a read well outside the buffers length
                if(y == 0)
                    continue;

                if(tiles.get(x, y - 1).id == tiles.get(x, y).id) {
                    tile_groups[cur_index].y += tile_groups[last_index].y;

                    tile_groups.erase(last_index);
                }
            }
        }

        // find tiles adjacent on the X-axis, this is the final step
        // to form the tile groups
        for(auto i = tile_groups.begin(); i != tile_groups.end(); i++) {
            glm::uvec2 coords       = tiles.get_2D_from_1D(i->first);
            uint32_t   current_tile = i->first;

            // We may iterate multiple times to combine groups 
            uint32_t   last_tile    = tiles.get_1D_from_2D(coords.x - tile_groups[current_tile].x, coords.y);

            if(tile_groups.find(last_tile) == tile_groups.end())
                continue;

            // if a tile has a different tile group height, the cant be added
            // the group wont be rectangular or sqaure, this will both
            // mess up the map, and cause it to render/ over other tiles
            if(tile_groups[last_tile].y != tile_groups[current_tile].y)
                continue;

            // self explantory; but if a tile group has a diff ID, it does not belong
            // to this tile group. A tile group must have tiles of the same ID
            if(tiles.get_from_1D(current_tile)->id != tiles.get_from_1D(last_tile)->id)
                continue;

            tile_groups[current_tile].x += tile_groups[last_tile].x;
            tile_groups.erase(last_tile);
            
            // the map may resize or move data around. 
            // var 'i' could be pointing towards bad data
            // so we 'reset'
            i = tile_groups.begin();
        }
    }

    void comp_tilemap_t::iterate_all(std::function<void(uint32_t x, uint32_t y)>&& clbk) {
        for(uint32_t x = 0; x < tiles.get_width(); x++) {
            for(uint32_t y = 0; y < tiles.get_height(); y++) {
                clbk(x, y);
            }
        }
    }

    void comp_tilemap_t::iterate_colliadable(std::function<void(uint32_t x, uint32_t y, tile_is_coll_info_t&)>&& clbk) {
        iterate_all([&](uint32_t x, uint32_t y) {
            tile_is_coll_info_t tile_info = tile_is_colliadable(x, y);
            
            if(tile_info.is_colliadable()) {
                clbk(x, y, tile_info);
            }
         });
    }
    
    void comp_tilemap_t::iterate_non_zero(std::function<void(uint32_t x, uint32_t y)>&& clbk) {
        iterate_all([&](uint32_t x, uint32_t y) {
            if(!is_tile_empty(tiles.get(x, y))) {
                clbk(x, y);
            }
        });
    }

    tile_is_coll_info_t comp_tilemap_t::tile_is_colliadable(uint32_t x, uint32_t y) {
        tile_is_coll_info_t info;

        if(spk::is_tile_empty(tiles.get(x, y))) // if its empty it can't collide
            return info;

       //  check left tile
        if(x - 1 != UINT32_MAX) {
            if(spk::is_tile_empty(tiles.get(x - 1, y))) {
                info.left = true;
            }
        } else { // edge tiles are colliadable
            info.left = true;
        }

        // check right tile
        if(x + 1 != tiles.get_width()) {
            if(spk::is_tile_empty(tiles.get(x  + 1, y))) {
                info.right = true;
            }
        } else { // edge tiles are colliadable
            info.right = true;
        }

        // check top tiles
        if(y - 1 != UINT32_MAX) {
            if(spk::is_tile_empty(tiles.get(x, y - 1))) {
                info.top = true;
            }
        } else { // edge tiles are colliadable
            info.top = true;
        }

        // check bottom tiles
        if(y + 1 != tiles.get_height()) {
            if(spk::is_tile_empty(tiles.get(x, y + 1))) {
                info.bottom = true;
            }
        } else { // edge tiles are colliadable
            info.bottom = true;
        }

        return info;
    }

    
    void comp_tilemap_t::compute_colliders() {
        compute_greedy_mesh();
        compute_centroid();
        colliding_tiles.clear();

        for(auto& pair : tile_groups) {
            glm::uvec2   coords = tiles.get_2D_from_1D(pair.first);
            tile_group_t tile   = pair.second;
            uint32_t     id     = tiles.get(coords.x, coords.y).id;

            if(is_tile_empty(tiles.get(coords.x, coords.y)))
               continue;

            {
                float  half_width    = tile.x / 2.0f;
                float  half_height   = tile.y / 2.0f;
                float  offset_width  = (coords.x + SPK_TILE_HALF_SIZE) - (float)tile.x  / 2.0f - center.x;
                float  offset_height = (coords.y + SPK_TILE_HALF_SIZE) - (float)tile.y  / 2.0f - center.y; 
                b2Vec2 offset        = { offset_width, offset_height };

                colliding_tiles.emplace_back();
                tile_collider_t& tile = colliding_tiles.back();

                b2Vec2 vertices[4] = {
                    b2Vec2(-half_width, -half_height) + offset,
                    b2Vec2(half_width, -half_height) + offset,
                    b2Vec2(half_width, half_height) + offset,
                    b2Vec2(-half_width, half_height) + offset
                };

                tile.shape.Set(vertices, 4);
                tile.id = id;
            }
        }
    }

    void comp_tilemap_t::compute_centroid() {
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

        width  = (right_most - left_most);
        height = (top_most - bottom_most);

        // adding by the half distance to find the center
        center.x = left_most   + width / 2.0f; 
        center.y = bottom_most + height / 2.0f; 
    }

    void comp_tilemap_t::update_tilemap() {
        void compute_greedy_mesh();
        void compute_centroid();
    }

    void comp_tilemap_t::add_fixtures(b2Body* body) {
        auto& dictionary = state.engine->rsrc_mng.get_tile_dictionary();

        update_tilemap();
        compute_colliders();

        for(auto& tile_collider : colliding_tiles) {
            b2FixtureDef def;
            def.density     = dictionary[tile_collider.id].density;
            def.friction    = dictionary[tile_collider.id].friction;
            def.restitution = dictionary[tile_collider.id].restitution;
            def.shape       = &tile_collider.shape;
            body->CreateFixture(&def);
        }
    }

    void tile_comp_init(flecs::world& world) {
        spk_register_component(world, comp_tilemap_t);
    }
}