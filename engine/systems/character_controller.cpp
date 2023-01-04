#include "character_controller.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void cc_mesh(flecs::iter& iter, comp_b2Body_t* bodies, comp_character_controller_t* ccs) {
        auto mesh = get_ctx<sprite_atlasd_batch_mesh_t>(iter);

        for(auto i : iter) {
            comp_b2Body_t*               body = &bodies[i];
            comp_character_controller_t* cc   = &ccs[i];            

            mesh->add_sprite_mesh(body->body, cc->sprite);
        }
    }

    void cc_tick(flecs::iter& iter, comp_b2Body_t* bodies, comp_character_controller_t* ccs) {
        for(auto i : iter) {
            comp_b2Body_t*               body  = &bodies[i];
            comp_character_controller_t* cc    = &ccs[i];
            b2Vec2                       force = {0.0f, 0.0f};

            if(state.engine->is_pressed(cc->left)) {
                force.x -= cc->speed;
            }    
            if(state.engine->is_pressed(cc->right)) {
                force.x += cc->speed;
            }    
            if(state.engine->is_pressed(cc->up)) {
                force.y += cc->speed;
            }    
            if(state.engine->is_pressed(cc->down)) {
                force.y -= cc->speed;
            }    

            body->body->ApplyLinearImpulseToCenter(force, true);
        }
    }

    void _character_controller_cs_init(mesh_t* mesh, flecs::world& world) {
        character_controller_comp_init(world);

        world.system<comp_b2Body_t, comp_character_controller_t>().iter(cc_tick);
        world.system<comp_b2Body_t, comp_character_controller_t>().ctx(mesh).kind(on_mesh).iter(cc_mesh);
    }
}