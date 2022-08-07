#include "physics.hpp"

namespace spk {
    void add_box2d_components(flecs::world& ecs_world, b2World* phy_world) {
        ecs_world.component<comp_b2Body>().on_add([&](flecs::entity e, comp_b2Body& comp){ // mmm complicated C++ types mmmmm
            comp.body = NULL_HANDLE;
        }).on_remove([=](flecs::entity e, comp_b2Body& comp){
            if(comp.body != NULL_HANDLE)
                phy_world->DestroyBody(comp.body);
        });
    }
} 