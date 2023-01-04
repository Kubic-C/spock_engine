#include "spock.hpp"
#include "utility/image_loader.hpp"

enum current_state_e: uint32_t {
    STATE_MENU,
    STATE_LOAD,
    STATE_PLAY,
    STATE_EXIT_PLAY,
    STATE_EXIT
};

void exit_game(spk::engine_t& engine, spk::ui_button_t& button) {
    *(current_state_e*)engine.user_state.user_data = STATE_EXIT;
}

void play_game(spk::engine_t& engine, spk::ui_button_t& button) {
    *(current_state_e*)engine.user_state.user_data = STATE_LOAD;
}

void exit_play_game(spk::engine_t& engine, spk::ui_button_t& button) {
    *(current_state_e*)engine.user_state.user_data = STATE_EXIT_PLAY;
}

MAIN {
    int exit_code = 0;
    spk::engine_t engine;
    flecs::entity canvas_e;
    spk::ui_button_t* exit_btn;
    spk::ui_button_t* play_btn;
    spk::ui_button_t* exit_play_btn;
    spk::ui_text_t* stats;
    current_state_e my_state;
    flecs::entity test, player;
    flecs::entity cam1, cam2;

    SPK_DEBUG_ENABLE(spk::DEBUG_FLAGS_ENABLE_STATE_CHANGE);
    SPK_DEBUG_ENABLE(spk::DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME);

    engine.init();

    srand(1);

    spk::log.log("[em, red] this text is red and emphasized [reset] ");
    spk::log.log("[it, blue] this text is italic and blue [reset]");
    spk::log.log("[emt] <- this is a recursive rule -> [emt]");

    cam1 = engine.world.entity();
    cam2 = engine.world.entity();

    cam1.set([&](spk::comp_camera_t& cam){
        cam.pos.x = -50.0f;
        cam.pos.y = -50.0f;
        cam.recalculate();
    });
    
    cam2.set([&](spk::comp_camera_t& cam){
        cam.pos.x  = 50.0f;
        cam.pos.y = 50.0f;
        cam.recalculate();
    });

    { // engine and window setup
        engine.set_target_fps(10000);
        engine.set_vsync_opt(spk::VSYNC_DISABLED);
        engine.set_current_window_title("hello world");
        engine.set_current_window_size(700, 700);

        b2World* world = engine.get_current_b2World();
        world->SetGravity(b2Vec2(0, -5.0f));
    }

    // resource management 
    {
        engine.rsrc_mng.font_load_ascii("./Anonymous.ttf");
        engine.rsrc_mng.atlas_init(0, 16, 16);
        if(!engine.rsrc_mng.atlas_load_from_path(0, "./test_atlas.png")) {
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

        engine.rsrc_mng.atlas_init(1, 8, 8);
        if(!engine.rsrc_mng.atlas_load_from_path(1, "./firetexture.png")) {
            spk::log.log("failed to load fire texture");
            return -101;
        }
    }
    
    { // UI setup
        canvas_e = engine.get_state().get_current_canvas();
        auto canvas = canvas_e.get_ref<spk::ui_comp_canvas_t>();

        exit_btn = canvas->btns.malloc();
        exit_btn->flags = spk::UI_ELEMENT_FLAGS_ENABLED | spk::UI_ELEMENT_FLAGS_RELATIVE;
        exit_btn->pos = { -0.4f, -0.8f };
        exit_btn->size = { -0.5f, -0.9f };
        exit_btn->callback = exit_game;
        canvas->add_child(exit_btn);

        play_btn  = canvas->btns.malloc();
        play_btn->flags = spk::UI_ELEMENT_FLAGS_ENABLED | spk::UI_ELEMENT_FLAGS_RELATIVE;
        play_btn->pos = { -0.4f, -0.55f };
        play_btn->size = { -0.5f, -0.9f };
        play_btn->callback = play_game;
        canvas->add_child(play_btn);

        exit_play_btn = canvas->btns.malloc();
        exit_play_btn->flags = spk::UI_ELEMENT_FLAGS_RELATIVE;
        exit_play_btn->pos  = { -0.4f, -0.8f };
        exit_play_btn->size = { -0.5f, -0.9f };
        exit_play_btn->callback = exit_play_game;
        canvas->add_child(exit_play_btn);

        spk::ui_text_t* txt = canvas->texts.malloc();
        txt->flags = spk::UI_ELEMENT_FLAGS_ENABLED | spk::UI_ELEMENT_FLAGS_PARENT_RELATIVE;
        txt->pos = { 0.0f, 0.0f };
        txt->text.set("exit", 1.0f, {0.0f, 0.0f, 0.0f});
        exit_btn->add_child(txt);

        spk::ui_text_t* txt2 = canvas->texts.malloc();
        txt2->flags = spk::UI_ELEMENT_FLAGS_ENABLED | spk::UI_ELEMENT_FLAGS_PARENT_RELATIVE;
        txt2->pos = { 0.0f, 0.0f };
        txt2->text.set("play", 1.0f, {0.0f, 0.0f, 0.0f});
        play_btn->add_child(txt2);

        spk::ui_text_t* exit_play_text = canvas->texts.malloc();
        exit_play_text->flags = spk::UI_ELEMENT_FLAGS_ENABLED | spk::UI_ELEMENT_FLAGS_PARENT_RELATIVE;
        exit_play_text->pos = { 0.0f, 0.0f };
        exit_play_text->text.set("exit to menu", 1.0f, {0.0f, 0.0f, 0.0f});
        exit_play_btn->add_child(exit_play_text);

        stats = canvas->texts.malloc();
        stats->flags = spk::UI_ELEMENT_FLAGS_ENABLED | spk::UI_ELEMENT_FLAGS_PARENT_RELATIVE;
        stats->pos = {0.0f, 0.95f};
        stats->text.set("stats ...", 1.0f, {0.0f, 0.0f, 0.0f});
        canvas->add_child(stats);

        my_state = STATE_MENU;
        engine.user_state.user_data = &my_state;
    }

    { // game setup
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
        td[4].default_flags = 0;
    }   

    auto start_ = [&]() -> void {
        for(uint32_t i = 0; i < 100; i++) {
            player = engine.world.entity();
            player.set([&](spk::comp_b2Body_t& body, spk::comp_character_controller_t& cc){
                cc.sprite.tax = 2;
                cc.sprite.z = -3.0f;

                b2BodyDef body_def;
                body_def.type = b2_dynamicBody;
                body_def.position = {(float)(rand() % 80) - 40.0f, 0.0f};
                body_def.bullet = false;
                body.body = engine.get_current_b2World()->CreateBody(&body_def);

                cc.add_fixture(body);
            });
        }

        test = engine.world.entity();
        test.add<spk::comp_body_prim_t>();
        test.set([&](spk::comp_particles_t& ps){
            ps.base_speed = 100.0f;
            ps.dir = {1.0f, 0.0f};
            ps.particle.id = 4;
        });
        test.set([&](spk::comp_b2Body_t& body, spk::comp_tilemap_t& comp){
            b2BodyDef def;
            body.body = engine.get_current_b2World()->CreateBody(&def);

            for(uint32_t x = 0; x < 100; x++) {
                for(uint32_t y = 0; y < 100; y++) {
                    if(x != 0 && x != (100 - 1) &&
                       y != 0 && y != (100 - 1) && y != 4) {
                        comp.tiles[x][y].id = 1;
                        comp.tiles[x][y].flags = 0;
                    } else {
                        comp.tiles[x][y].id = 2;
                        comp.tiles[x][y].flags = spk::TILE_FLAGS_COLLIADABLE;
                    }
                }
            }

            comp.add_fixtures(body.body);
        });
    };

    auto end_ = [&](){
        spk::log.log("ending sim");
        test.destruct();
        player.destruct();
    };

    engine.world.observer().event<spk::event_mouse_wheel_t>().term<spk::tag_events_t>()
        .iter([&](flecs::iter& iter){
            auto event = iter.param<spk::event_mouse_wheel_t>();
            auto camera = engine.get_current_camera();
            float scale = 0.1;

            if(event->y < 0) {
                camera->scale -= scale;
            } else if(event->y > 0) {
                camera->scale += scale;
            }
        });

    engine.user_state.tick = 
        [&](spk::engine_t& engine){
            auto stats_ = engine.get_stats();
            stats->text.set(std::string("stats: FPS: ") + 
                            std::to_string(stats_.fps) + " | TPS: " + 
                            std::to_string(stats_.tps) + " | DT: " +
                            std::to_string(stats_.average_delta_time) + " | FT: " +
                            std::to_string(stats_.frame_time) + " | Physics bodies: " +
                            std::to_string(engine.world.count<spk::comp_b2Body_t>()), 0.4f, {1.0f, 1.0f, 1.0f});

            switch(my_state) {
            case STATE_LOAD:
                play_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_play_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
            
                start_();
                my_state = STATE_PLAY;
                
                spk::log.log("loading state");
                engine.set_target_fps(1000000);
                break;

            case STATE_PLAY: {
                auto cam = engine.get_current_camera();
                float speed = 2.5f;

                player.set([&](spk::comp_b2Body_t& body) {
                    cam->pos = body.get_pos();
                });
            } break;

            case STATE_EXIT_PLAY:
                engine.set_target_fps(60);
                play_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_play_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;

                my_state = STATE_MENU;

                end_();

                spk::log.log("exiting play");
                break;

            case STATE_EXIT:
                engine.exit(0);
                spk::log.log("exiting app");
                break;

            default:
                break;
            }
        };

    exit_code = engine.run();
    engine.free();

    return exit_code;
}