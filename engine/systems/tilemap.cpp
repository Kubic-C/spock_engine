#include "tilemap.hpp"
#include "core/internal.hpp"
#include "components/rigid_body.hpp"
#include "sprite.hpp"

namespace spk {
    // void tilemap_mesh(
    //     sprite_arrayd_batch_mesh_t* mesh, 
    //     b2Body*                     body, 
    //     comp_tilemap_t&             tilemap) {

    //     for(auto& pair : tilemap.tile_groups) {
    //         glm::uvec2   coords = tilemap.tiles.get_2D_from_1D(pair.first);
    //         tile_group_t tile   = pair.second;
    //         auto&        sprite = internal->resources.tile_dictionary[tilemap.tiles.get(coords.x, coords.y).id].sprite;
    //         float        offset_width;
    //         float        offset_height;

    //         if(tilemap.tiles.get(coords.x, coords.y).id == 0)
    //            continue;

    //         offset_width  = (coords.x + SPK_TILE_HALF_SIZE) - (float)tile.x  / 2.0f - tilemap.center.x;
    //         offset_height = (coords.y + SPK_TILE_HALF_SIZE) - (float)tile.y  / 2.0f - tilemap.center.y; 

    //         mesh->add_sprite_mesh(body, sprite, tile.x, tile.y, (glm::vec2){offset_width, offset_height});
    //     }
    // }

    // void sprite_render_system_tilebody_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_tilemap_t* tilemaps) {
    //     auto ctx = get_ctx<sprite_arrayd_batch_mesh_t>(iter);

    //     for(auto i : iter) {
    //         comp_rigid_body_t&  body     = bodies[i];
    //         comp_tilemap_t& tilemap = tilemaps[i];

    //         tilemap_mesh(ctx, body, tilemap);
    //     }
    // }

    void tilemap_cs_init(flecs::world& world) {
        spk_trace();
        
        tile_comp_init(world);

        // world.system<comp_rigid_body_t, comp_tilemap_t>().kind(on_mesh_id).ctx(ctx).iter(sprite_render_system_tilebody_mesh);
    }
}