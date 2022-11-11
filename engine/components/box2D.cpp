#include "box2D.hpp"
#include "../state.hpp"

namespace spk {
    void comp_b2Body_on_add(comp_b2Body_t& comp) {
        comp.body = nullptr;       
    }

    void comp_b2Body_on_remove(comp_b2Body_t& comp) {
        if(comp.body)
            comp.body->GetWorld()->DestroyBody(comp.body);
    }

    void comp_b2Body_init(flecs::world& world) {
        world.component<comp_b2Body_t>();
        world.observer<comp_b2Body_t>("OnAdd b2Body observer").event(flecs::OnAdd).each(comp_b2Body_on_add);
        world.observer<comp_b2Body_t>("OnRemove b2Body observer").event(flecs::OnRemove).each(comp_b2Body_on_remove);
    }

    void comp_box2d_world_on_add(comp_box2d_world_t& world) {
        world.world = new b2World(b2Vec2(0.0f, -9.81f * 10));
    }

    void comp_box2d_world_on_remove(comp_box2d_world_t& world) {
        delete world.world;
    }

    void tag_current_box2d_world_on_add(flecs::entity e, tag_current_box2d_world_t tag) {
        if(state._get_current_box2D_world().id() != UINT64_MAX) {
            state._get_current_box2D_world().remove<tag_current_box2d_world_t>();
        }

        sfk_assert(e.has<comp_box2d_world_t>());
        state._set_current_box2D_world(e);
    }

    void comp_box2d_world_init(flecs::world& world) {
        world.component<comp_box2d_world_t>();
        world.observer<tag_current_box2d_world_t>("OnAdd Current b2World tag observer").event(flecs::OnAdd).each(tag_current_box2d_world_on_add);
        world.observer<comp_box2d_world_t>("OnAdd b2World observer").event(flecs::OnAdd).each(comp_box2d_world_on_add);
        world.observer<comp_box2d_world_t>("OnRemove b2World observer").event(flecs::OnRemove).each(comp_box2d_world_on_remove);
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