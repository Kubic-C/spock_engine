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
    constexpr size_t tile_chunk_width  = 8;
    constexpr size_t tile_chunk_height = 8;
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
        kin::fixture_t* fixture = nullptr;
    };

    struct tile_chunk_info_t {
        bool empty          = false;
        bool mesh_dirty     = true;
        bool fixtures_dirty = true;
        std::map<uint32_t, mesh_t<tilemap_vertex_t, 6, 4>> buffer;
    };

    typedef array2D_t<tile_t, tile_chunk_width, tile_chunk_height> tile_chunk_t;
    typedef array2D_t<tile_chunk_t, tilemap_max_width, tilemap_max_height> tilemap_chunks_t;
    typedef array2D_t<tile_chunk_info_t, tilemap_max_width, tilemap_max_height> tilemap_chunk_info_t;

    struct comp_tilemap_t : component_t {
        ptm::ptr_t<tilemap_chunk_info_t> infos;
        ptm::ptr_t<tilemap_chunks_t>     chunks;

        void init(flecs::entity entity);
        void free(flecs::entity entity);

        tile_t& get(size_t x, size_t y, bool tile_change = true) {
            const size_t xtile  = x % tile_chunk_width;
            const size_t ytile  = y % tile_chunk_height;
            const size_t xchunk = (x - xtile) / tile_chunk_width;
            const size_t ychunk = (y - ytile) / tile_chunk_height;
        
            (*infos)[xchunk][ychunk].fixtures_dirty = true;
            (*infos)[xchunk][ychunk].mesh_dirty = true;

            return (*chunks)[xchunk][ychunk][xtile][ytile];
        }

        void mesh();
        void add_fixtures(kin::rigid_body_t* body);

    private:
        void mesh_tiles(uint32_t xc, uint32_t yc);
        void add_tile_fixtures(kin::rigid_body_t* body, uint32_t xc, uint32_t yc);
    };

    void tile_comp_init(const flecs::world& world);
}