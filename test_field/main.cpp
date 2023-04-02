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

    spk::print_deps_versions();

    spk::settings_t&  settings  = spk::settings();
    spk::resources_t& resources = spk::resources();
    spk::scene_t&     scene     = spk::scene();
    spk::canvas_t&    canvas    = spk::canvas();
    b2World*          world = scene.physics_world;
    uint32_t          smells_blood_id, coin_sound_id;
    std::array<uint32_t, 5> fonts;
    uint32_t          sprite_array_id, high_def_array;
    spk::ptr_t<spk::text_t> text;

    {
        smells_blood_id = spk::music_create("smells_blood.mp3");
        coin_sound_id   = spk::chunk_create("coin_sound.wav");
        // spk::music_play(smells_blood_id, 1);

        spk::audio_chunk_volume(1);
        spk::audio_music_volume(1);
        world->SetGravity(b2Vec2(0.0f, 0.0f));

        fonts[0] = spk::font_create("./fonts/DotGothic16-Regular.ttf", 128, 20);
        fonts[1] = spk::font_create("./fonts/eater.regular.ttf", 128, 20);
        fonts[2] = spk::font_create("./fonts/FiraCode-Bold.ttf", 128, 20);
        fonts[3] = spk::font_create("./fonts/GajrajOne-Regular.ttf", 128, 20);
        fonts[4] = spk::font_create("./fonts/PressStart2P.ttf", 128, 20);

        sprite_array_id = spk::sprite_array_create(32, 32, 5);
        spk::sprite_array_set(sprite_array_id, "./texture_array/image1.png", 0);
        spk::sprite_array_set(sprite_array_id, "./texture_array/image2.png", 1);
        spk::sprite_array_set(sprite_array_id, "./texture_array/image3.png", 2);

        high_def_array = spk::sprite_array_create(1000, 1000, 2);
        spk::sprite_array_set(high_def_array, "./texture_array/windows_logo.png", 0);

        spk::tile_dictionary_t& td = resources.tile_dictionary;
        
        td[1].sprite.id   = sprite_array_id;
        td[1].sprite.z    = -5.0f;
        td[1].restitution = 1.1f;
        td[1].friction    = 0.0f;

        td[2].sprite.id = sprite_array_id;
        td[2].sprite.index = 1;
        td[2].sprite.z = 1.0f;
        td[2].density = 5.0f;
        td[2].friction = 0.0f;
        td[2].restitution = 0.0f;

        td[3].sprite.id = sprite_array_id;
        td[3].sprite.z = 1.0f;
        td[3].sprite.index = 2;
        td[3].density = 500.0f;
    }

    { // canvas 
        canvas.font = fonts[0];

        text = canvas.element<spk::text_t>();

        text->x_set(spk::constraint_relative(0.05f));
        text->y_set(spk::constraint_relative(0.85f));
        text->width_set(spk::constraint_relative(0.5f));
        text->height_set(spk::constraint_relative(0.1f));
        text->text       = "loading..";
        text->text_color = {1.0f, 1.0f, 1.0f};
        text->color      = {1.0f, 1.0f, 1.0f, 0.0f};

        spk::ptr_t button = text->element<spk::button_t>();
        button->x_set(spk::constraint_center());
        button->y_set(spk::constraint_relative(0.0f));
        button->width_set(spk::constraint_relative(0.3f));
        button->height_set(spk::constraint_relative(0.2f));
        button->color       = {1.0f, 0.0f, 0.0f, 1.0f};        
        button->hover_color = {1.0f, 0.4f, 0.4f};
        button->click_color = {1.0f, 0.5f, 0.5f};    
        button->callback = [&](spk::button_t& button) {
            spk::exit();
        };

        spk::ptr_t button_text = button->element<spk::text_t>();
        button_text->x_set(spk::constraint_center());
        button_text->y_set(spk::constraint_relative(0.0f));
        button_text->width_set(spk::constraint_relative(1.0f));
        button_text->height_set(spk::constraint_relative(1.0f));
        button_text->color = {0.0f, 0.0f, 0.0f, 0.0f};
        button_text->text  = "click button to exit";

        spk::settings().target_tps = 60;
        spk::settings().target_fps = 120;
    }

    for(size_t i = 0; i < 10; i++) {
        scene.ecs_world.entity().set([&](spk::comp_rigid_body_t& rb, spk::comp_sprite_t& sprite, spk::comp_particles_t& ps) {
            float random = rand();
            random *= 0.01f;

            rb->SetType(b2_dynamicBody);
            rb->SetBullet(true);
            rb->SetTransform((glm::vec2){fmod(random, 100.0f) - 50.0f, 100.0f}, 0.0f);

            b2PolygonShape shape;
            float hl = fmod(random, 2.5f) + 0.4f;
            shape.SetAsBox(hl, hl);

            b2FixtureDef fdef;
            fdef.shape = &shape;
            fdef.density = 0.5f;
            fdef.restitution = 0.1f;
            rb->CreateFixture(&fdef);

            sprite.size = {hl, hl};
            sprite.id = 1;
            sprite.index = 1;
            sprite.z = 1.0f;

            ps.funnel = spk::PARTICLES_FUNNEL_FUNNEL;
            ps.step   = 0.2f; // fun :)
            ps.length = 1.0f;
            ps.base_speed = 9.0f;
            ps.sprite = 3;
            ps.base_cycle = 0.5f;
            ps.base_lifetime = 1.0f;
            ps.max = UINT32_MAX;
            ps.flags  = spk::PARTICLES_FLAGS_WORLD_DIRECTION | spk::PARTICLES_FLAGS_WORLD_POSITION | spk::PARTICLES_FLAGS_ACTIVE;
        });
    }
    
    scene.ecs_world.entity().set([&](spk::comp_rigid_body_t& rb, spk::comp_tilemap_t& tm){
        rb->SetType(b2_dynamicBody);
        tm.tiles.size(500, 20);

        for(uint32_t x = 0; x < tm.tiles.get_width(); x++) {
            for(uint32_t y = 0; y < tm.tiles.get_height(); y++) {
                if(5 < y && y < 15) {
                    tm.tiles.get(x, y).id = 2 + (rand() % 2);
                    tm.tiles.get(x, y).flags = spk::TILE_FLAGS_COLLIADABLE;
                }
            }
        }

        tm.add_fixtures(rb);
    });

    scene.ecs_world.entity().set([&](spk::comp_rigid_body_t& rb, spk::comp_tilemap_t& tm){
        rb->SetType(b2_staticBody);
        tm.tiles.size(1000, 1000);

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
            spk::comp_tilemap_t& tilemap,
            spk::comp_camera_t& camera) {
        rb->SetType(b2_dynamicBody);
        rb->SetBullet(true);

        tilemap.tiles.get(1, 0) = 3;
        tilemap.tiles.get(1, 1) = 3;
        tilemap.tiles.get(1, 2) = 3;
        tilemap.tiles.get(2, 1) = 3;

        tilemap.add_fixtures(rb);
        rb->SetTransform(b2Vec2(0.0f, 40.0f), 0.0f);

        cc.speed = 10000.0f;

        b2CircleShape shape;
        shape.m_radius = 50.0f;
        b2FixtureDef fixture;
        fixture.shape = &shape;
        fixture.isSensor = true;
        rb->CreateFixture(&fixture);
    
        callbacks.begin =[&](flecs::entity self, flecs::entity other, b2Contact* contact) {            
            b2Fixture* self_fixture;
            b2Fixture* other_fixture;

            if(self == contact->GetEntityA()) {
                self_fixture = contact->GetFixtureA();
                other_fixture = contact->GetFixtureB();
            } else {
                self_fixture = contact->GetFixtureB();
                other_fixture = contact->GetFixtureA();
            }

            if(!self_fixture->IsSensor()) {
                spk::chunk_play(coin_sound_id, 0);
                return;
            } else {
                glm::vec2 dir_away = 
                    glm::normalize((glm::vec2)other_fixture->GetBody()->GetPosition() - (glm::vec2)self_fixture->GetBody()->GetPosition());

                float strength = 1000.0f;

                other_fixture->GetBody()->ApplyLinearImpulseToCenter(-dir_away * strength, true);
            }
        };
    });

    character.add<spk::tag_current_camera_t>();
    scene.camera.get_ref<spk::comp_camera_t>()->scale = 1.5f;

    scene.user_data.update = [&](){
        if(spk::window().key_get(SDL_SCANCODE_2)) {
            canvas.font = fonts[0];
        }
        else if(spk::window().key_get(SDL_SCANCODE_3)) {
            canvas.font = fonts[1];
        }
        else if(spk::window().key_get(SDL_SCANCODE_4)) {
            canvas.font = fonts[2];
        }
        else if(spk::window().key_get(SDL_SCANCODE_5)) {
            canvas.font = fonts[3];
        }
        else if(spk::window().key_get(SDL_SCANCODE_6)) {
            canvas.font = fonts[4];
        }

        character.set([&](spk::comp_rigid_body_t& rb, spk::comp_camera_t& camera){
            camera.pos = rb->GetPosition();

            text->text = std::string(spk::build_name()) + " | FPS:" + std::to_string(spk::statistics().fps) + " | TPS:"  + std::to_string(spk::statistics().tps);
            text->text += "\nvelocity.xy:" + std::to_string(rb->GetLinearVelocity().x) + " / " + std::to_string(rb->GetLinearVelocity().y);
        
            if(spk::window().key_get(SDL_SCANCODE_1)) {
                rb->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
            }
        });
    };  

    code = spk::run();

    spk::free();

    return code;
}