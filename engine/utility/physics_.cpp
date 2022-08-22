#include "physics_.hpp"

namespace spk {
    void add_box2d_components(flecs::world& ecs_world, b2World* phy_world) {
        ecs_world.component<comp_b2Body>().on_add([&](flecs::entity e, comp_b2Body& comp){ // mmm complicated C++ types mmmmm
            comp.body = NULL_HANDLE;
        }).on_remove([=](flecs::entity e, comp_b2Body& comp){
            if(comp.body != NULL_HANDLE)
                phy_world->DestroyBody(comp.body);
        });
    }

    b2Fixture* add_body_fixture(comp_b2Body* body, b2Shape* shape, float friction, float restitution,
		float restitutionThreshold, float density, bool isSensor) {

        b2FixtureDef def;
        def.shape = shape;
        def.friction = friction;
        def.restitution = restitution;
        def.restitutionThreshold = restitutionThreshold;
        def.density = density;
        def.isSensor = isSensor;
        return body->body->CreateFixture(&def);
    }
} 