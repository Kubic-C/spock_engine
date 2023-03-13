#include "character_controller.hpp"
#include "components/rigid_body.hpp"
#include "core/internal.hpp"

namespace spk {
    void cc_tick(flecs::iter& iter, comp_rigid_body_t* bodies, comp_character_controller_t* ccs) {
        window_t* window = internal->scene.window;

        for(auto i : iter) {
            comp_rigid_body_t&           body  = bodies[i];
            comp_character_controller_t* cc    = &ccs[i];
            glm::vec2                    force = {0.0f, 0.0f};

            if(window->get_key_state(cc->left)) {
                force.x -= cc->speed;
            }    
            if(window->get_key_state(cc->right)) {
                force.x += cc->speed;
            }    
            if(window->get_key_state(cc->up)) {
                force.y += cc->speed;
            }    
            if(window->get_key_state(cc->down)) {
                force.y -= cc->speed;
            }    

            body->ApplyLinearImpulseToCenter(force, false);
        }
    }

    void _character_controller_cs_init(mesh_t* mesh, flecs::world& world) {
        spk_trace();
        
        character_controller_comp_init(world);

        world.system<comp_rigid_body_t, comp_character_controller_t>().iter(cc_tick);
    }
}