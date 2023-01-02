#include "tilemap.hpp"
#include "sprite.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void sprite_render_system_tilebody_mesh(flecs::iter& iter, comp_b2Body_t* bodies, comp_tilemap_t* tilemaps) {
        auto ctx = SPK_GET_CTX(iter, sprite_batch_mesh_t);
        resource_manager_t* rsrc_mng = &state.engine->rsrc_mng;

        for(auto i : iter) {
            comp_b2Body_t&  body     = bodies[i];
            comp_tilemap_t& tilemap = tilemaps[i];

            spk_assert(tilemap.tiles.size() == tilemap.size.x &&
                       tilemap.tiles[0].size() == tilemap.size.y);

            for(uint32_t x = 0; x < tilemap.size.x; x++) {
                for(uint32_t y = 0; y < tilemap.size.y; y++) {
                    tile_t& tile          = tilemap.tiles[x][y];
                    comp_sprite_t& sprite = rsrc_mng->get_tile_dictionary()[tile.id].sprite;

                    if(tile.id != 0) {
                        ctx->add_sprite_mesh(body, sprite, (glm::vec2){x, y} - tilemap.center);
                    }
                }
            }
        }

    }

    void _tilemap_cs_init(sprite_batch_mesh_t* ctx, flecs::world& world) {
        tile_comp_init(world);

        world.system<comp_b2Body_t, comp_tilemap_t>().kind(on_mesh).ctx(ctx).iter(sprite_render_system_tilebody_mesh);
    }
}