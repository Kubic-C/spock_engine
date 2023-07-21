/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "tilemap.hpp"
#include "core/internal.hpp"
#include "components/rigid_body.hpp"
#include "sprite.hpp"

namespace spk {
    void chunk_update(comp_tilemap_t& tilemap, size_t xc, size_t yc) {
        auto chunks = tilemap.chunks;
        auto infos  = tilemap.infos;

        bool all_empty = true;

        if((*tilemap.infos)[xc][yc].empty)
            return;

        for(uint32_t xt = 0; xt < (*chunks)[xc][yc].width_get(); xt++) {
            for(uint32_t yt = 0; yt < (*chunks)[xc][yc].height_get(); yt++) {
                tile_t& tile = (*chunks)[xc][yc][xt][yt];

                if(!tile.empty()) {
                    all_empty = false;
                }

                if(tile.animation.last_switch + tile.animation.switch_time < spk::time.get_time() &&
                    tile.animation.on) {
                    if(tile.sprite.index >= tile.animation.index_end) {
                        tile.sprite.index = tile.animation.index_begin;
                    } else {
                        tile.sprite.index++;
                    }

                    (*infos)[xc][yc].mesh_dirty = true;
                    tile.animation.last_switch = spk::time.get_time();
                }
            }
        }

        (*tilemap.infos)[xc][yc].empty = all_empty;
    }

    void tilemap_system(flecs::iter& iter, comp_rigid_body_t* bodies, comp_tilemap_t* tilemaps) {
        for(auto i : iter) {
            comp_rigid_body_t& body    = bodies[i];
            comp_tilemap_t&    tilemap = tilemaps[i];
        
            tilemap.add_fixtures(body);

            for(uint32_t xc = 0; xc < tilemap.chunks->width_get(); xc++) {
                for(uint32_t yc = 0; yc < tilemap.chunks->width_get(); yc++) {
                    chunk_update(tilemap, xc, yc);
                }
            }
        }
    }

    void tilemap_cs_init(const flecs::world& world) {
        spk_trace();
        
        tile_comp_init(world);

        world.system<comp_rigid_body_t, comp_tilemap_t>().iter(tilemap_system);
    }
}