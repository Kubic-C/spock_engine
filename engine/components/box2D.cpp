#include "box2D.hpp"
#include "../state.hpp"

namespace spk {
    void comp_b2Body_t::init() {
        body = nullptr;       
    }

    void comp_b2Body_t::free() {
        if(body)
            body->GetWorld()->DestroyBody(body);
    }

    void comp_b2World_t::init() {
        world = new b2World(b2Vec2(0.0f, -9.81f * 10));
    }

    void comp_b2World_t::free() {
        delete world;
    }

    void box2d_world_tag_on_add(flecs::entity e, comp_b2World_t& world, tag_current_box2d_world_t tag) {
        if(state.get_current_box2D_world().id() != UINT64_MAX) {
            state.get_current_box2D_world().remove<tag_current_box2d_world_t>();
        }

        state.set_current_box2D_world(e);
    }

    void box2d_comp_init(flecs::world& world) {
        sfk_register_component(world, comp_b2Body_t);
        sfk_register_component(world, comp_b2World_t);

        world.observer<comp_b2World_t, tag_current_box2d_world_t>("OnAdd Current b2World tag observer")
            .event(flecs::OnAdd).each(box2d_world_tag_on_add);
    }

    b2Fixture* add_body_fixture(comp_b2Body_t* body, b2Shape* shape, float friction, float restitution,
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