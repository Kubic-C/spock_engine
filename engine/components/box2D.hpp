#pragma once

#include "base.hpp"

namespace spk {
    struct tag_current_box2d_world_t {};

    struct comp_b2World_t {
        b2World* world;

        void init();
        void free();
    };

    struct comp_b2Body_t { 
        b2Body* body;

        glm::vec2 get_pos() {
            return to_glm_vec2(body->GetPosition());
        }

        glm::vec2 get_world_point(float x, float y) {
            return to_glm_vec2(body->GetWorldPoint((b2Vec2){x, y}));
        }

        void init();
        void free();
    };  

    void box2d_comp_init(flecs::world& world); // init all comps

    b2Fixture* add_body_fixture(comp_b2Body_t* body, b2Shape* shape, float friction = 0.2f, float restitution = 0.0f,
		float restitutionThreshold = 1.0f, float density = 0.0f, bool isSensor = false);
}