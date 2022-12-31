#include "sprite.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void sprite_render_pre_mesh(flecs::iter& iter) {
        auto sprite_renderer = SPK_GET_CTX_REF(iter, sprite_batch_mesh_t);

        for(auto& mesh : sprite_renderer->meshes) {
            mesh.sprites = 0;
        }
    }

    void sprite_render_system_update(flecs::iter& iter, comp_b2Body_t* bodies, comp_sprite_t* sprites) {
        auto sprite_renderer = SPK_GET_CTX_REF(iter, sprite_batch_mesh_t);
        resource_manager_t* rsrc_mng = &state.engine->rsrc_mng;

        for(auto i : iter) {
            comp_b2Body_t&  body = bodies[i];
            comp_sprite_t&  sprite = sprites[i];

            sprite_renderer->add_sprite_mesh(body.body, sprite);
        }
    }
    void sprite_cs_init(system_ctx_allocater_t& allocater, flecs::world& world) {
        sprite_comp_init(world);

        auto sr = allocater.allocate_ctx<sprite_renderer_t>();

        state.get_current_renderer()->rp_add_renderer(0, (base_renderer_t*)sr);

        world.system().kind(on_pre_mesh)
            .ctx(&sr->sprites).iter(sprite_render_pre_mesh);
        world.system<comp_b2Body_t, comp_sprite_t>().kind(on_mesh)
            .ctx(&sr->sprites).iter(sprite_render_system_update);

        _particles_cs_init(&sr->sprites, world);        
        _tilebody_cs_init(&sr->sprites, world); 
    }
}