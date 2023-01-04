#include "tilemap.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    struct tile_size_t {
        uint32_t x = 1, y = 1;
    };

    void sprite_render_system_tilebody_mesh(flecs::iter& iter, comp_b2Body_t* bodies, comp_tilemap_t* tilemaps) {
        auto ctx = get_ctx<sprite_arrayd_batch_mesh_t>(iter);
        resource_manager_t* rsrc_mng = &state.engine->rsrc_mng;

        for(auto i : iter) {
            comp_b2Body_t&  body     = bodies[i];
            comp_tilemap_t& tilemap = tilemaps[i];

            spk_assert(tilemap.tiles.size() == tilemap.size.x &&
                       tilemap.tiles[0].size() == tilemap.size.y);

            std::array<std::array<tile_size_t, 100>, 100> tile_sizes;

            // find tile sizes on the y
            for(uint32_t x = 0; x < tilemap.size.x; x++) {
                for(uint32_t y = 0; y < tilemap.size.y; y++) {
                    if(y != 0) {
                        if(tilemap.tiles[x][y - 1].id == tilemap.tiles[x][y].id) {
                            tile_sizes[x][y].y += tile_sizes[x][y - 1].y;
                        }
                    }
                }
            }

            for(int32_t x = tilemap.size.x - 1; x >= 0; x--) {
                for(int32_t y = tilemap.size.y - 1; y >= 0;) {
                    tile_size_t&   tile   = tile_sizes[x][y];
                    auto& sprite = rsrc_mng->get_tile_dictionary()[tilemap.tiles[x][y].id].sprite;
                    float offset_width, offset_height;

                    if(tilemap.tiles[x][y].id == 0)
                        goto next;

                    offset_width  = (float)x - tilemap.center.x;
                    offset_height = (y + SPK_TILE_HALF_SIZE) - (float)tile.y  / 2.0f - tilemap.center.y; 

                    ctx->add_sprite_mesh(body.body, sprite, tile.x, tile.y, (glm::vec2){offset_width, offset_height});

                next:
                    y -= tile.y;
                }
            }
        }

    }

    void _tilemap_cs_init(mesh_t* ctx, flecs::world& world) {
        tile_comp_init(world);

        world.system<comp_b2Body_t, comp_tilemap_t>().kind(on_mesh).ctx(ctx).iter(sprite_render_system_tilebody_mesh);
    }
}