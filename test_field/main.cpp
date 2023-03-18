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

    SPK_DEBUG_ENABLE(spk::DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME);

    spk::init();

    spk::settings_t&  settings  = spk::settings();
    spk::resources_t& resources = spk::resources();
    spk::scene_t&     scene     = spk::scene();
    uint32_t smells_blood_id, coin_sound_id;

    settings.box2d_draw_flags = b2Draw::e_shapeBit;

    b2World* world = scene.physics_world;

    {
        smells_blood_id = spk::music_load("smells_blood.mp3");
        coin_sound_id   = spk::chunk_load("coin_sound.wav");

        resources.fonts.load_ascii("./Anonymous.ttf");
        resources.sprite_atlases.init(0, 16, 16);
        if(!resources.sprite_atlases.load_from_path(0, "./test_atlas.png")) {
            spk::log.log("failed to load atlas");
            return -100;
        }
        
        resources.sprite_arrays.init(0);
        resources.sprite_arrays.start(0, 32, 32, 5);
        resources.sprite_arrays.load(0, "./texture_array/image1.png", 0);
        resources.sprite_arrays.load(0, "./texture_array/image2.png", 1);
        resources.sprite_arrays.finish(0);

        resources.sprite_arrays.init(1);
        resources.sprite_arrays.start(1, 32, 32, 5);
        resources.sprite_arrays.load(1, "./texture_array/image3.png", 0);
        resources.sprite_arrays.finish(1);

        spk::tile_dictionary_t& td = resources.tile_dictionary;
        
        td[1].sprite.z      = -5.0f;
        td[1].restitution = 1.1f;
        td[1].friction = 0.0f;

        td[2].sprite.array_id = 0;
        td[2].sprite.index = 1;
        td[2].density = 50.0f;
        td[2].friction = 0.0f;
        td[2].restitution = 0.0f;

        td[4].sprite.array_id = 1;
        td[4].sprite.index = 0;
        td[4].density = -5.0f;
    }

    for(size_t i = 0; i < 10; i++) {
        scene.ecs_world.entity().set([&](spk::comp_rigid_body_t& rb, spk::comp_sprite_t& sprite) {
            float random = rand();
            random *= 0.01f;

            rb->SetType(b2_dynamicBody);
            rb->SetBullet(true);
            rb->SetTransform((glm::vec2){fmod(random, 10.0f) - 5.0f, fmod(random, 10.0f) - 5.0f}, 0.0f);

            b2PolygonShape shape;
            float hl = fmod(random, 5.0f) + 0.1f;
            shape.SetAsBox(hl, hl);

            b2FixtureDef fdef;
            fdef.shape = &shape;
            fdef.density = 0.5f;
            fdef.restitution = 0.1f;
            rb->CreateFixture(&fdef);

            sprite.size = {hl, hl};
            sprite.array_id = 0;
            sprite.index = 1;
            sprite.z = 0.0f;
        });
    }
    
    flecs::entity wall = scene.ecs_world.entity().set([&](spk::comp_rigid_body_t& rb, spk::comp_tilemap_t& tm){
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

    flecs::entity character = scene.ecs_world.entity().set([&](
            spk::comp_rigid_body_t& rb, 
            spk::comp_character_controller_t& cc, 
            spk::comp_contact_callback_t& callbacks,
            spk::comp_tilemap_t& tilemap) {
        rb->SetType(b2_dynamicBody);
        rb->SetBullet(true);

        tilemap.tiles.get(1, 0) = 2;
        tilemap.tiles.get(1, 1) = 2;
        tilemap.tiles.get(1, 2) = 2;
        tilemap.tiles.get(1, 3) = 2;
        tilemap.tiles.get(1, 4) = 2;
        tilemap.tiles.get(1, 5) = 2;
        tilemap.tiles.get(1, 6) = 2;
        tilemap.tiles.get(1, 7) = 2;
        tilemap.tiles.get(1, 8) = 2;
        tilemap.tiles.get(1, 9) = 2;
        tilemap.tiles.get(1, 10) = 2;
        tilemap.tiles.get(1, 11) = 2;

        tilemap.add_fixtures(rb);

        cc.speed = 100.0f;

        b2CircleShape shape;
        shape.m_radius = 15.0f;
        b2FixtureDef fixture;
        fixture.shape = &shape;
        fixture.isSensor = true;
        rb->CreateFixture(&fixture);
    
        callbacks.end = [&](flecs::entity self, flecs::entity other, b2Contact* contact) {
            spk::chunk_play(coin_sound_id, 0);

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

                float strength = 100000.0f;

                other_fixture->GetBody()->ApplyLinearImpulseToCenter(-dir_away * strength, true);
            }
        };
    }).add<spk::tag_body_render_t>();

    scene.camera.get_ref<spk::comp_camera_t>()->scale = 1.5f;

    scene.user_data.update = [&](){
        character.set([&](spk::comp_rigid_body_t& rb){
            scene.camera.get_ref<spk::comp_camera_t>()->pos = rb->GetPosition();
            rb->ApplyTorque(1000000, true);
        });
    };  

    code = spk::run();

    spk::free();

    return code;
}