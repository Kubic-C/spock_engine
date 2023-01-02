#include "sprite.hpp"
#include "state.hpp" 

namespace spk {
    b2Fixture* comp_sprite_t::add_fixture(b2Body* body, float density, float friction, float restitution) {
        b2PolygonShape shape;
        shape.SetAsBox(size.x, size.y);

        b2FixtureDef fix_def;
        fix_def.density     = density;
        fix_def.friction    = friction;
        fix_def.restitution = restitution;
        fix_def.shape       = &shape;

        return body->CreateFixture(&fix_def);        
    }

    void sprite_comp_init(flecs::world& world) {
        spk_register_component(world, comp_sprite_t);
    }
}