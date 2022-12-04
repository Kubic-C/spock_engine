#pragma once

#include "framework.hpp"

namespace spk {
    struct tag_current_box2d_world_t {};

    struct comp_box2d_world_t {
        b2World* world;

        comp_box2d_world_t();
        ~comp_box2d_world_t();
    };

    struct comp_b2Body_t { 
        b2Body* body;

        comp_b2Body_t();
        ~comp_b2Body_t();
    };  

    void tag_current_box2d_world_on_add(flecs::entity e, tag_current_box2d_world_t tag);
    void comp_box2d_world_init(flecs::world& world);

    b2Fixture* add_body_fixture(comp_b2Body_t* body, b2Shape* shape, float friction = 0.2f, float restitution = 0.0f,
		float restitutionThreshold = 1.0f, float density = 0.0f, bool isSensor = false);
}