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
        mesh   = allocators().chunk_mesh_pool.create(1);
    }

    void comp_tilemap_t::free(flecs::entity entity) {
        for(int x = 0; x < mesh->width_get(); x++) {
            for(int y = 0; y < mesh->height_get(); y++) {
                for(auto& group : (*mesh)[x][y].groups) {
                    group.fixture = nullptr;
                }
            }
        }

        allocators().chunk_mesh_pool.destruct(mesh.get(), 1);
        allocators().chunks_pool.destruct(chunks.get(), 1);
    }

    void chunk_column_mesh_update(tile_chunk_t& chunk, tile_chunk_mesh_t& mesh, size_t column) {
        size_t contigous_amount = 0;
        tile_t contigous_tile   = UINT32_MAX;        

        size_t y = 0;
        for(; y < tile_chunk_height; y++) {
        check:
            if(chunk[column][y] == contigous_tile) {
                contigous_amount++;
            } else if(contigous_tile == UINT32_MAX) {
                contigous_tile = chunk[column][y];
                contigous_amount++;
            } else if(chunk[column][y] != contigous_tile) {
                if(contigous_tile != 0U) {
                    mesh.groups.emplace_back((tile_group_t){
                        .tile   = contigous_tile,
                        .total  = 1 * contigous_amount,
                        .size   = {1, contigous_amount},
                        .offset = {column * SPK_TILE_FULL_SIZE, (y - contigous_amount) * SPK_TILE_FULL_SIZE}
                    });      
                }

                contigous_amount = 0;
                contigous_tile     = UINT32_MAX;
                goto check; // have to check the current tile
            }
        }

        // for loop does not add the last tile group
        if(contigous_tile != 0 && contigous_tile != UINT32_MAX) {
            mesh.groups.emplace_back((tile_group_t){
                .tile   = contigous_tile,
                .total  = 1 * contigous_amount,
                .size   = {1, contigous_amount},
                .offset = {column * SPK_TILE_FULL_SIZE, (y - contigous_amount) * SPK_TILE_FULL_SIZE}
            });     
        }
    }

    void chunk_mesh_update(kin::rigid_body_t* body, comp_tilemap_t& tilemap, size_t xchunk, size_t ychunk) {
        tile_chunk_t&      chunk = (*tilemap.chunks)[xchunk][ychunk];
        tile_chunk_mesh_t& mesh  = (*tilemap.mesh)[xchunk][ychunk]; 

        if(!mesh.dirty)
            return;

        mesh.dirty = false;
        mesh.groups.clear();
                        
        for(auto& pair : mesh.buffer) {
            pair.second.vertices_on_buffer = 0;
            pair.second.vertices_to_render = 0;
        }

        for(size_t xtile = 0; xtile < tile_chunk_width; xtile++) {
            chunk_column_mesh_update(chunk, mesh, xtile);
        }

        for(tile_group_t& group : mesh.groups) {
            const float id    = tile_dictionary()[group.tile.id].sprite.id;
            const float index = tile_dictionary()[group.tile.id].sprite.index;
            const float z     = tile_dictionary()[group.tile.id].sprite.z;
            const float xbase = (xchunk * tile_chunk_width * SPK_TILE_FULL_SIZE) + group.offset.x;
            const float ybase = (ychunk * tile_chunk_height * SPK_TILE_FULL_SIZE) + group.offset.y;
            const float xsize = group.size.x * SPK_TILE_FULL_SIZE;
            const float ysize = group.size.y * SPK_TILE_FULL_SIZE;
            
            // render vertices
            tilemap_vertex_t vertices[] = {
                (glm::vec3){xbase        , ybase, z}, (glm::vec3){0.0f        , 0.0f        , index},
                (glm::vec3){xbase + xsize, ybase, z}, (glm::vec3){group.size.x, 0.0f        , index},
                (glm::vec3){xbase + xsize, ybase + ysize, z}, (glm::vec3){group.size.x, group.size.y, index},
                (glm::vec3){xbase        , ybase + ysize, z}, (glm::vec3){0.0f        , group.size.y, index}
            };

            mesh.buffer[(uint32_t)id].add_mesh(vertices);

            // physic vertices
            if(group.tile.flags & TILE_FLAGS_COLLIADABLE) {
                kin::fixture_def_t def;
                def.hw      = xsize * 0.5f;
                def.hh      = ysize * 0.5f;
                def.rel_pos = (glm::vec2){xbase + def.hw, ybase + def.hh};
                def.density     = tile_dictionary()[group.tile.id].density * group.total;
                def.restitution = tile_dictionary()[group.tile.id].restitution;
                def.static_friction = tile_dictionary()[group.tile.id].friction;
                def.dynamic_friction = tile_dictionary()[group.tile.id].friction;
                group.fixture = body->create_fixture(def);
            }
        }
    }

    void _tilemap_mesh_update(comp_rigid_body_t* body, comp_tilemap_t& tilemap) {
        tilemap_mesh_t& mesh = *tilemap.mesh;

        for(size_t xchunk = 0; xchunk < mesh.width_get(); xchunk++) {
            for(size_t ychunk = 0; ychunk < mesh.height_get(); ychunk++) {
                chunk_mesh_update(body->body, tilemap, xchunk, ychunk);
            }
        }
    }

    void tile_comp_init(const flecs::world& world) {
        spk_register_component(world, comp_tilemap_t);
    }
}