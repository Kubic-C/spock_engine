#include "tilebody.hpp"
#include "sprite.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void sprite_render_system_tilebody_update(flecs::iter& iter, comp_tilebody_t* tilebodies) {
        auto ctx = SPK_GET_CTX_REF(iter, sprite_batch_mesh_t);
        resource_manager_t* rsrc_mng = &state.engine->rsrc_mng;

        for(auto i : iter) {
            comp_tilebody_t& tilebody = tilebodies[i];

            for(uint32_t x = 0; x < tilebody.tilemap.size.x; x++) {
                for(uint32_t y = 0; y < tilebody.tilemap.size.y; y++) {
                    tile_t& tile = tilebody.tilemap.tiles[x][y];

                    if(tile.id != 0) {
                        ctx->add_sprite_mesh(tilebody.body, rsrc_mng->get_tile_dictionary()[tile.id].sprite, 
                            (glm::vec2){x, y} - tilebody.tilemap.center);
                    }
                }
            }
        }
    }

    void _tilebody_cs_init(sprite_batch_mesh_t* ctx, flecs::world& world) {
        tile_comp_init(world);

        world.system<comp_tilebody_t>().kind(on_mesh).ctx(ctx).iter(sprite_render_system_tilebody_update);
    }
}