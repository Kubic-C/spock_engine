#include "sprite.hpp"
#include "core/internal.hpp"
#include "components/rigid_body.hpp"

namespace spk {
    void sprite_render_init(flecs::world& world) {
        sprite_comp_init(world);


        // pre mesh
        world.system().kind(on_pre_mesh_id)
            .ctx(&sr->atlasd_sprites).iter(sprite_atlas_mesh_pre_mesh);
        world.system().kind(on_pre_mesh_id)
            .ctx(&sr->arrayd_sprites).iter(sprite_array_mesh_pre_mesh);

        // on mesh
        world.system<comp_rigid_body_t, comp_sprite_atlasd_t>().kind(on_mesh_id)
            .ctx(&sr->atlasd_sprites).iter(sprite_atlas_mesh);
        world.system<comp_rigid_body_t, comp_sprite_arrayd_t>().kind(on_mesh_id)
            .ctx(&sr->arrayd_sprites).iter(sprite_array_mesh);
    }
}