/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "components.hpp"
#include "utility/array2D.hpp"
#include "core/tiles.hpp"
#include "render/common.hpp"

namespace spk {
    constexpr size_t tile_chunk_width  = 16;
    constexpr size_t tile_chunk_height = 16;
    constexpr size_t tilemap_max_width    = 16;
    constexpr size_t tilemap_max_height   = 16;
    constexpr size_t tilemap_tile_width   = tile_chunk_width * tilemap_max_width;
    constexpr size_t tilemap_tile_height  = tile_chunk_height * tilemap_max_height;

    struct tilemap_vertex_t {
        glm::vec3 pos;
        glm::vec3 tex;
    };

    struct tile_group_t {
        ~tile_group_t();

        tile_t     tile;
        size_t     total;
        glm::vec2  size;
        glm::vec2  offset;
        b2Fixture* fixture = nullptr;
    };

    struct tile_chunk_mesh_t {
        bool                                               dirty = true;
        std::vector<tile_group_t>                          groups;
        std::map<uint32_t, mesh_t<tilemap_vertex_t, 6, 4>> buffer;
    };

    typedef array2D_t<tile_t, tile_chunk_width, tile_chunk_height> tile_chunk_t;
    typedef array2D_t<tile_chunk_t, tilemap_max_width, tilemap_max_height> tilemap_chunks_t;
    typedef array2D_t<tile_chunk_mesh_t, tilemap_max_width, tilemap_max_height> tilemap_mesh_t;

    struct comp_tilemap_t : component_t {
        void init(flecs::entity entity);
        void free(flecs::entity entity);

        void set(uint32_t id, size_t x, size_t y, uint8_t flags = TILE_FLAGS_COLLIADABLE) {
            spk_assert(x < tilemap_tile_width && y < tilemap_tile_height);
            spk_assert(!chunks.is_null());

            const size_t xtile  = x % tile_chunk_width;
            const size_t ytile  = y % tile_chunk_height;
            const size_t xchunk = (x - xtile) / tile_chunk_width;
            const size_t ychunk = (y - ytile) / tile_chunk_height;

            set(id, xchunk, ychunk, xtile, ytile, flags);
        }

        void set(uint32_t id, size_t xchunk, size_t ychunk, size_t xtile, size_t ytile, uint8_t flags) {
            spk_assert(xchunk < tilemap_max_width && ychunk < tilemap_max_height);
            spk_assert(xtile < tile_chunk_width && ytile < tile_chunk_height);
            spk_assert(!chunks.is_null());

            (*mesh)[xchunk][ychunk].dirty = true;
            (*chunks)[xchunk][ychunk][xtile][ytile].id = id;
            (*chunks)[xchunk][ychunk][xtile][ytile].flags = flags;
        }

        ptr_t<tilemap_mesh_t>   mesh;
        ptr_t<tilemap_chunks_t> chunks;
    };

    void tile_comp_init(const flecs::world& world);
    void _tilemap_mesh_update(b2Body* body, comp_tilemap_t& tilemap);
}