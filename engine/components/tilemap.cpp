/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "tilemap.hpp"
#include "core/internal.hpp"

namespace spk {
    tile_group_t::~tile_group_t() {
        if(fixture)
            fixture->body->destroy_fixture(fixture);
    }
    
    void comp_tilemap_t::init(flecs::entity entity) {
        chunks = allocators().chunks_pool.create(1);
        infos  = allocators().chunk_info_pool.create(1);
    }

    void comp_tilemap_t::free(flecs::entity entity) {
        for(auto& chunk : (*chunks)) {
            for(auto& tile : chunk) {
                tile.fixture = nullptr;
            }
        }

        allocators().chunk_info_pool.destruct(infos.get(), 1);
        allocators().chunks_pool.destruct(chunks.get(), 1);
    }

    void comp_tilemap_t::mesh_tiles(uint32_t xc, uint32_t yc) {
        tile_chunk_info_t& info = (*infos)[xc][yc];

        if(!info.mesh_dirty)
            return;

        info.mesh_dirty = false;
        info.empty      = false;

        for(auto& chunks : info.buffer) {
            chunks.second.vertices_on_buffer = 0;
            chunks.second.vertices_to_render = 0;
        }

        for(uint32_t xt = 0; xt < (*chunks)[xc][yc].width_get(); xt++) {
            for(uint32_t yt = 0; yt < (*chunks)[xc][yc].height_get(); yt++) {
                size_t  x = (xc * (*chunks)[xc][yc].width_get()) + xt;
                size_t  y = (yc * (*chunks)[xc][yc].height_get()) + yt;
                tile_t& tile = (*chunks)[xc][yc][xt][yt];

                if(!tile.is_renderable()) {
                    continue;
                }

                tilemap_vertex_t vertices[] = {
                    (glm::vec3){x    , y    , tile.sprite.z}, (glm::vec3){0.0f, 0.0f, tile.sprite.index},
                    (glm::vec3){x + 1, y    , tile.sprite.z}, (glm::vec3){1.0f, 0.0f, tile.sprite.index},
                    (glm::vec3){x + 1, y + 1, tile.sprite.z}, (glm::vec3){1.0f, 1.0f, tile.sprite.index},
                    (glm::vec3){x    , y + 1, tile.sprite.z}, (glm::vec3){0.0f, 1.0f, tile.sprite.index}
                };

                info.buffer[tile.sprite.id].add_mesh(vertices);
            }
        }
    }

    void comp_tilemap_t::mesh() {
        for(uint32_t xc = 0; xc < chunks->width_get(); xc++) {
            for(uint32_t yc = 0; yc < chunks->width_get(); yc++) {
                mesh_tiles(xc, yc);
            }
        }
    }

    void comp_tilemap_t::add_tile_fixtures(kin::rigid_body_t* body, uint32_t xc, uint32_t yc) {
        tile_chunk_info_t& info = (*infos)[xc][yc];

        if(!info.fixtures_dirty)
            return;
        
        info.fixtures_dirty = false;
        info.empty          = false;

        for(uint32_t xt = 0; xt < (*chunks)[xc][yc].width_get(); xt++) {
            for(uint32_t yt = 0; yt < (*chunks)[xc][yc].height_get(); yt++) {
                size_t  x    = (xc * (*chunks)[xc][yc].width_get()) + xt;
                size_t  y    = (yc * (*chunks)[xc][yc].height_get()) + yt;
                tile_t& tile = (*chunks)[xc][yc][xt][yt];

                if(tile.fixture) {
                    body->destroy_fixture(tile.fixture);
                } 

                if(tile.flags & TILE_FLAGS_COLLIADABLE) {
                    kin::fixture_def_t fdef;
                    fdef.hw               = SPK_TILE_HALF_SIZE;
                    fdef.hh               = SPK_TILE_HALF_SIZE;
                    fdef.rel_pos          = (glm::vec2){x + fdef.hw, y + fdef.hh};
                    fdef.density          = tile.density;
                    fdef.restitution      = tile.restitution;
                    fdef.static_friction  = tile.friction;
                    fdef.dynamic_friction = tile.friction;
                    tile.fixture = body->create_fixture(fdef);
                }
            }
        }
    }

    void comp_tilemap_t::add_fixtures(kin::rigid_body_t* body) {
        for(uint32_t xc = 0; xc < chunks->width_get(); xc++) {
            for(uint32_t yc = 0; yc < chunks->width_get(); yc++) {
                add_tile_fixtures(body, xc, yc);
            }
        }
    }

    void tile_comp_init(const flecs::world& world) {
        spk_register_component(world, comp_tilemap_t);
    }
}