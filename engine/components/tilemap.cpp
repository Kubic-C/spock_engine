#include "tilemap.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void comp_tilemap_t::init() {
        tiles.init(100, 100);

        for(uint32_t x = 0; x < tiles.get_width(); x++) {
            for(uint32_t y = 0; y < tiles.get_height(); y++) {
                tiles.get(x, y).id = 0;
                tiles.get(x, y).flags = TILE_FLAGS_COLLIADABLE;
            }
        }
    }

    void comp_tilemap_t::free() {
        tiles.free();
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
                // it will not be added to the map at all, causing it not to 
                // be added to the tile groups
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
            // var 'I' could be pointing towards bad data
            // so we 'reset'
            i = tile_groups.begin();
        }
    }

    void comp_tilemap_t::iterate_map(std::function<void(uint32_t x, uint32_t y)>&& clbk) {
        for(uint32_t x = 0; x < tiles.get_width(); x++) {
            for(uint32_t y = 0; y < tiles.get_height(); y++) {
                clbk(x, y);
            }
        }
    }

    void comp_tilemap_t::iterate_colliadable(std::function<void(uint32_t x, uint32_t y, tile_is_coll_info_t&)>&& clbk) {
        iterate_map([&](uint32_t x, uint32_t y) {
            tile_is_coll_info_t tile_info = tile_is_colliadable(x, y);
            
            if(tile_info.is_colliadable()) {
                clbk(x, y, tile_info);
            }
         });
    }
    
    void comp_tilemap_t::iterate_non_zero(std::function<void(uint32_t x, uint32_t y)>&& clbk) {
        iterate_map([&](uint32_t x, uint32_t y) {
            if(!is_tile_empty(tiles.get(x, y))) {
                clbk(x, y);
            }
        });
    }

    tile_is_coll_info_t comp_tilemap_t::tile_is_colliadable(uint32_t x, uint32_t y) {
        tile_is_coll_info_t info;

        if(spk::is_tile_empty(tiles.get(x, y))) // if its empty it can't collide
            return info;

        // check left tile
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
        float half_width  = (float)tiles.get_width()  / 2.0f;
        float half_height = (float)tiles.get_height() / 2.0f;

        compute_greedy_mesh();
        compute_centroid();
        colliding_tiles.clear();

        iterate_colliadable([&](uint32_t x, uint32_t y, tile_is_coll_info_t& info){
            colliding_tiles.push_back((tile_collider_t){.id = tiles.get(x, y).id});
            auto& shape = colliding_tiles.back().shape;

            shape.SetAsBox(SPK_TILE_HALF_SIZE, SPK_TILE_HALF_SIZE);
            shape.m_vertices[0] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
            shape.m_vertices[1] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
            shape.m_vertices[2] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
            shape.m_vertices[3] += (b2Vec2){(float)x, (float)y} - spk::to_box_vec2(center);
        });

        iterate_non_zero([&](uint32_t x, uint32_t y) {
            tile_t& tile = tiles.get(x, y);
            tile_metadata_t& md = state.engine->rsrc_mng.get_tile_dictionary()[tile.id];
            
            // Compute the mass for a given tile
            const float area = SPK_TILE_HALF_SIZE * SPK_TILE_HALF_SIZE; // in sqaure meters
            mass += area * md.density;
        });
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

        // adding by the half distance to find the center
        center.x = left_most   + (right_most - left_most) / 2.0f; 
        center.y = bottom_most + (top_most - bottom_most) / 2.0f; 
    }

    void comp_tilemap_t::add_fixtures(b2Body* body) {
        auto& dictionary = state.engine->rsrc_mng.get_tile_dictionary();

        compute_colliders();

        for(auto& tile_collider : colliding_tiles) {
            b2FixtureDef def;
            def.density     = dictionary[tile_collider.id].density;
            def.friction    = dictionary[tile_collider.id].friction;
            def.restitution = dictionary[tile_collider.id].restitution;
            def.shape       = &tile_collider.shape;
            body->CreateFixture(&def);
        }

        b2MassData md = body->GetMassData();
        md.center = to_box_vec2(center);
        md.mass   = mass;
        //md.I      = 1.0f;

        body->SetMassData(&md);
    }

    void tile_comp_init(flecs::world& world) {
        spk_register_component(world, comp_tilemap_t).is_a<comp_b2Body_t>();
    }
}