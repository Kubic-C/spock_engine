#include "state.hpp"
#include "spock.hpp"
#include "utility/image_loader.hpp"

enum current_state_e: uint32_t {
    STATE_MENU,
    STATE_LOAD,
    STATE_PLAY,
    STATE_EXIT_PLAY,
    STATE_EXIT
};

MAIN {
    spk_trace();

    spk::log.log("running..");
    int code = 0;

    spk::engine_t engine;

    SPK_DEBUG_ENABLE(spk::DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME);

    engine.init();
    engine.set_box2d_draw_flags(b2Draw::e_centerOfMassBit);

    b2World* world = engine.get_current_physics_world();

    {
        engine.rsrc_mng.font_load_ascii("./Anonymous.ttf");
        engine.rsrc_mng.sprite_atlas_init(0, 16, 16);
        if(!engine.rsrc_mng.sprite_atlas_load_from_path(0, "./test_atlas.png")) {
            spk::log.log("failed to load atlas");
            return -100;
        }
        
        engine.rsrc_mng.sprite_array_init(0);
        engine.rsrc_mng.sprite_array_start(0, 32, 32, 5);
        engine.rsrc_mng.sprite_array_load(0, "./texture_array/image1.png", 0);
        engine.rsrc_mng.sprite_array_load(0, "./texture_array/image2.png", 1);
        engine.rsrc_mng.sprite_array_finish(0);

        engine.rsrc_mng.sprite_array_init(1);
        engine.rsrc_mng.sprite_array_start(1, 32, 32, 5);
        engine.rsrc_mng.sprite_array_load(1, "./texture_array/image3.png", 0);
        engine.rsrc_mng.sprite_array_finish(1);

        spk::tile_dictionary_t& td = engine.rsrc_mng.get_tile_dictionary();
        
        td[1].sprite.z      = -5.0f;
        td[1].restitution = 1.1f;
        td[1].friction = 0.0f;

        td[2].sprite.index = 1;
        td[2].density = 50.0f;
        td[2].friction = 0.0f;
        td[2].restitution = 0.0f;

        td[4].sprite.array_id = 1;
        td[4].sprite.index = 0;
        td[4].density = -5.0f;
    }

    for(size_t i = 0; i < 100; i++) {
        engine.world->entity().set([&](spk::comp_rigid_body_t& rb) {
            float random = rand();
            random *= 0.01f;

            rb->SetType(b2_dynamicBody);
            rb->SetTransform((glm::vec2){fmod(random, 10.0f) - 5.0f, fmod(random, 10.0f) - 5.0f}, 0.0f);

            b2CircleShape shape;

            shape.m_radius = fmod(random, 1.9f) + 0.1f;

            b2FixtureDef fdef;
            fdef.shape = &shape;
            fdef.density = 0.1f;
            fdef.restitution = 1.0f;
            rb->CreateFixture(&fdef);
        });
    }
    
    flecs::entity wall = engine.world->entity().set([&](spk::comp_rigid_body_t& rb, spk::comp_tilemap_t& tm){
        rb->SetType(b2_staticBody);

        for(uint32_t x = 0; x < tm.tiles.get_width(); x++) {
            for(uint32_t y = 0; y < tm.tiles.get_height(); y++) {
                if(x != 0 && x != (tm.tiles.get_width() - 1) &&
                    y != 0 && y != (tm.tiles.get_height() - 1)) {
                    tm.tiles.get(x, y).id = 1;
                    tm.tiles.get(x, y).flags = 0;
                } else {
                    tm.tiles.get(x, y).id = 2;
                    tm.tiles.get(x, y).flags = spk::TILE_FLAGS_COLLIADABLE;
                }
            }
        }

        tm.add_fixtures(rb);
    });

    flecs::entity character = engine.world->entity().set([&](
            spk::comp_rigid_body_t& rb, 
            spk::comp_character_controller_t& cc, 
            spk::comp_contact_callback_t& callbacks,
            spk::comp_tilemap_t& tilemap) {
        rb->SetType(b2_dynamicBody);

        tilemap.tiles.get(0, 0) = 2;
        tilemap.tiles.get(1, 1) = 2;
        tilemap.tiles.get(1, 2) = 2;
        tilemap.tiles.get(1, 3) = 2;
        tilemap.tiles.get(1, 4) = 2;
        tilemap.tiles.get(1, 0) = 2;

        tilemap.add_fixtures(rb);

        cc.speed = 100.0f;
    
       callbacks.end = [&](flecs::entity self, flecs::entity other, b2Contact* contact) {
            if(other == wall)
                return;
            
            b2Fixture* self_fixture;
            b2Fixture* other_fixture;

            if(self == contact->GetEntityA()) {
                self_fixture = contact->GetFixtureA();
                other_fixture = contact->GetFixtureB();
            } else {
                self_fixture = contact->GetFixtureB();
                other_fixture = contact->GetFixtureA();
            }

            if(!self_fixture->IsSensor())
                return;

            {
                glm::vec2 dir_away = 
                    glm::normalize((glm::vec2)other_fixture->GetBody()->GetPosition() - (glm::vec2)self_fixture->GetBody()->GetPosition());

                float strength = 100.0f;

                other_fixture->GetBody()->ApplyLinearImpulseToCenter(dir_away * strength, true);
            }
        };
    });

    engine.get_current_camera()->scale = 1.5f;

    engine.user_state.update = [&](spk::engine_t& engine){
        character.set([&](spk::comp_rigid_body_t& rb){
            engine.get_current_camera()->pos = rb->GetPosition();
        });
    };  

    code = engine.run();

    engine.free();

    return code;
}