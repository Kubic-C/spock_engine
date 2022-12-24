#include "tilebody.hpp"
#include "../state.hpp"
#include "../spock.hpp"

namespace spk {
    void comp_tilebody_t::init() {
        auto box_world = state.engine->get_current_b2World();

        tilemap.init();

        b2BodyDef body_def;
        body_def.type = b2_staticBody;
        body_def.position =  {0.0f, 0.0f};
        this->body = box_world->CreateBody(&body_def);
    }

    void comp_tilebody_t::free() {
        auto box_world = state.engine->get_current_b2World();
       
        tilemap.free();

        box_world->DestroyBody(this->body); 
    }

    void comp_tilebody_t::add_fixtures() {
        auto& dictionary = state.engine->rsrc_mng.get_tile_dictionary();

        tilemap.find_colliding_tiles();

        for(auto& tile_collider : tilemap.colliding_tiles) {
            auto& tile = tilemap.tiles[tile_collider.index.x][tile_collider.index.y];

            b2FixtureDef def;
            def.density = dictionary[tile.id].density;
            def.friction = dictionary[tile.id].friction;
            def.restitution = dictionary[tile.id].restitution;
            def.shape = &tile_collider.shape;
            body->CreateFixture(&def);
        }
    }

    void tile_comp_init(flecs::world& world) {
        sfk_register_component(world, comp_tilebody_t);
    }
}