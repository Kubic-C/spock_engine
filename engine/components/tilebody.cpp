#include "tilebody.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void comp_tilebody_t::init() {
        auto box_world = state.engine->get_current_b2World();

        tilemap.init();

        b2BodyDef body_def;
        body_def.type = b2_staticBody;
        this->body = box_world->CreateBody(&body_def);
    }

    void comp_tilebody_t::free() {
        auto box_world = state.engine->get_current_b2World();
       
        tilemap.free();

        box_world->DestroyBody(this->body); 
    }

    void comp_tilebody_t::add_fixtures() {
        auto& dictionary = state.engine->rsrc_mng.get_tile_dictionary();

        tilemap.compute_colliders();

        for(auto& tile_collider : tilemap.colliding_tiles) {
            b2FixtureDef def;
            def.density     = dictionary[tile_collider.id].density;
            def.friction    = dictionary[tile_collider.id].friction;
            def.restitution = dictionary[tile_collider.id].restitution;
            def.shape       = &tile_collider.shape;
            body->CreateFixture(&def);
        }

        b2MassData md;
        md.center = to_box_vec2(tilemap.center);
        md.mass   = tilemap.mass;
        md.I      = 0.0f;

        body->SetMassData(&md);
    }

    void tile_comp_init(flecs::world& world) {
        spk_register_component(world, comp_tilebody_t).is_a<comp_b2Body_t>();
    }
}