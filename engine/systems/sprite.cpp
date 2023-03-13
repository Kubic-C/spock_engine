#include "sprite.hpp"
#include "core/internal.hpp"
#include "components/rigid_body.hpp"

namespace spk {
    void sprite_atlas_mesh_pre_mesh(flecs::iter& iter) {
        auto sprite_mesh = get_ctx<sprite_atlasd_batch_mesh_t>(iter);

        for(auto& mesh : sprite_mesh->meshes) {
            mesh.sprites = 0;
        }
    }

    void sprite_array_mesh_pre_mesh(flecs::iter& iter) {
        auto sprite_mesh = get_ctx<sprite_arrayd_batch_mesh_t>(iter);

        for(auto& mesh : sprite_mesh->meshes) {
            mesh.sprites = 0;
        }
    }

    void sprite_atlas_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_sprite_atlasd_t* sprites) {
        auto sprite_mesh = get_ctx<sprite_atlasd_batch_mesh_t>(iter);

        for(auto i : iter) {
            comp_rigid_body_t&  body = bodies[i];
            comp_sprite_atlasd_t&  sprite = sprites[i];

            sprite_mesh->add_sprite_mesh(body, sprite);
        }
    }

    
    void sprite_array_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_sprite_arrayd_t* sprites) {
        auto sprite_mesh = get_ctx<sprite_arrayd_batch_mesh_t>(iter);

        for(auto i : iter) {
            comp_rigid_body_t&  body = bodies[i];
            comp_sprite_arrayd_t&  sprite = sprites[i];

            sprite_mesh->add_sprite_mesh(body, sprite);
        }
    }
    
    void sprite_cs_init(flecs::world& world) {
        sprite_comp_init(world);

        auto sr = internal->allocators.stack.push<sprite_renderer_t>();

        internal->scene.renderer->rp_add_renderer(0, (base_renderer_t*)sr);

        // pre mesh
        world.system().kind(on_pre_mesh)
            .ctx(&sr->atlasd_sprites).iter(sprite_atlas_mesh_pre_mesh);
        world.system().kind(on_pre_mesh)
            .ctx(&sr->arrayd_sprites).iter(sprite_array_mesh_pre_mesh);

        // on mesh
        world.system<comp_rigid_body_t, comp_sprite_atlasd_t>().kind(on_mesh)
            .ctx(&sr->atlasd_sprites).iter(sprite_atlas_mesh);
        world.system<comp_rigid_body_t, comp_sprite_arrayd_t>().kind(on_mesh)
            .ctx(&sr->arrayd_sprites).iter(sprite_array_mesh);

        _particles_cs_init(&sr->arrayd_sprites, world);        
        _tilemap_cs_init(&sr->arrayd_sprites, world);
        _character_controller_cs_init(&sr->atlasd_sprites, world); 
    }
}