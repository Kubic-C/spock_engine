#include "tilemap.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    struct tile_size_t {
        uint32_t x = 1, y = 1;
    };

    void greedy_tilemap_mesh(
        sprite_arrayd_batch_mesh_t* mesh, 
        b2Body*                     body, 
        comp_tilemap_t&             tilemap, 
        resource_manager_t*         rsrc_mng) {

        std::map<uint32_t, tile_size_t> sizes = {};
        array2D_t<tile_t>&              tiles = tilemap.tiles;

        // find tile sizes on the y
        for(uint32_t x = 0; x < tilemap.size.x; x++) {
            for(uint32_t y = 1; y < tilemap.size.y; y++) {
                uint32_t cur_index = tiles.get_1D_from_2D(x, y);

                if(tiles.get(x, y - 1).id == tiles.get(x, y).id) {
                    sizes[cur_index].y += 1;
                }
            }
        }

        // find tile widths
        for(auto pair : sizes) {
            glm::uvec2 coords       = tiles.get_2D_from_1D(pair.first);
            uint32_t   last_tile    = tiles.get_1D_from_2D(coords.x - 1, coords.y);
            uint32_t   current_tile = pair.first;

            if(sizes.find(last_tile) == sizes.end())
                continue;

            if(sizes[last_tile].y != sizes[current_tile].y)
                continue;

            if(tiles.get_from_1D(current_tile)->id != tiles.get_from_1D(last_tile)->id)
                continue;

            sizes[current_tile].x += sizes[last_tile].x;
            sizes.erase(last_tile);
        }

        for(auto& pair : sizes) {
            glm::uvec2  coords = tiles.get_2D_from_1D(pair.first);
            tile_size_t tile   = pair.second;
            auto&       sprite = rsrc_mng->get_tile_dictionary()[tiles.get(coords.x, coords.y).id].sprite;
            float       offset_width;
            float       offset_height;

            if(tiles.get(coords.x, coords.y).id == 0)
               continue;

            offset_width  = (coords.x + SPK_TILE_HALF_SIZE) - (float)tile.x  / 2.0f - tilemap.center.x;
            offset_height = (coords.y + SPK_TILE_HALF_SIZE) - (float)tile.y  / 2.0f - tilemap.center.y; 

            mesh->add_sprite_mesh(body, sprite, tile.x, tile.y, (glm::vec2){offset_width, offset_height});
        }
    }

    void sprite_render_system_tilebody_mesh(flecs::iter& iter, comp_b2Body_t* bodies, comp_tilemap_t* tilemaps) {
        auto ctx = get_ctx<sprite_arrayd_batch_mesh_t>(iter);
        resource_manager_t* rsrc_mng = &state.engine->rsrc_mng;

        for(auto i : iter) {
            comp_b2Body_t&  body     = bodies[i];
            comp_tilemap_t& tilemap = tilemaps[i];

            greedy_tilemap_mesh(ctx, body.body, tilemap, rsrc_mng);
        }

    }

    void _tilemap_cs_init(mesh_t* ctx, flecs::world& world) {
        tile_comp_init(world);

        world.system<comp_b2Body_t, comp_tilemap_t>().kind(on_mesh).ctx(ctx).iter(sprite_render_system_tilebody_mesh);
    }
}