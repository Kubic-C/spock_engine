#include "spock.hpp"

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

// TODO:
// Tick and update for user side now becomes intergated, and seperate from flecs
// fix on_add being called after flecs::entity::set

MAIN {
    int exit_code = 0;
    spk::engine_t engine;
    flecs::entity e;
    flecs::entity canvas_e;
    spk::ui_button_t* exit_btn;
    spk::ui_button_t* play_btn;
    spk::ui_button_t* exit_play_btn;
    current_state_e my_state;
    flecs::entity test, bottom;
    flecs::entity cam1, cam2;

    spk::debug.flags |= spk::DEBUG_FLAGS_ENABLE_STATE_CHANGE | spk::DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME | spk::DEBUG_FLAGS_ENABLE_HOOK;

    engine.init();
    
    srand(1);

    sfk::log.log("[em, red] this text is red and emphasized [reset] ");
    sfk::log.log("[it, blue] this text is italic and blue [reset]");
    sfk::log.log("[emt] <- this is a recursive rule -> [emt]");

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
        engine.set_vsync_opt(spk::VSYNC_ENABLED);
        engine.set_current_window_title("hello world");
        engine.set_current_window_size(700, 700);

        b2World* world = engine.get_current_b2World();
        world->SetGravity(b2Vec2(0.0f, -9.81f));
    }

    // resource management 
    {
        engine.rsrc_mng.font_load_ascii("./Raleway-Regular.ttf");
        engine.rsrc_mng.atlas_init(0, 16, 16);
        if(!engine.rsrc_mng.atlas_load_from_path(0, "./test_atlas.png")) {
            sfk::log.log("failed to load atlas");
            return -100;
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

        my_state = STATE_MENU;
        engine.user_state.user_data = &my_state;
    }

    { // game setup
        spk::tile_dictionary_t& td = engine.rsrc_mng.get_tile_dictionary();
        
        td[1].sprite.tax = 6;
        td[1].sprite.tay = 3;

        td[2].sprite.tax = 0;
        td[2].density = 5.0f;
    }   

    auto start_ = [&]() -> void {
            test = engine.world.entity();
            test.set([&](spk::comp_tilebody_t& comp){
                uint32_t id = (rand() % 2) + 1;

                comp.tilemap.tiles =
                    { std::vector<spk::tile_t>({0, 1, 1, 1, 1, 0, 0, 0, 0, 0}), 
                      std::vector<spk::tile_t>({0, 1, 1, 1, 1, 0, 0, 0, 0, 0}),  
                      std::vector<spk::tile_t>({0, 1, 1, 1, 1, 0, 0, 0, 0, 0}), 
                      std::vector<spk::tile_t>({0, 1, 1, 1, 1, 0, 0, 0, 0, 0}), 
                      std::vector<spk::tile_t>({0, 1, 1, 1, 1, 1, 1, 1, 1, 1}), 
                      std::vector<spk::tile_t>({0, 0, 0, 0, 1, 0, 0, 0, 0, 1}),
                      std::vector<spk::tile_t>({0, 0, 0, 0, 1, 0, 0, 0, 0, 1}),
                      std::vector<spk::tile_t>({0, 0, 0, 0, 1, 0, 0, 0, 0, 1}), 
                      std::vector<spk::tile_t>({0, 0, 0, 0, 1, 0, 0, 0, 0, 1}), 
                      std::vector<spk::tile_t>({0, 0, 0, 0, 1, 0, 0, 0, 0, 2}), 
                    };

                comp.add_fixtures();
                comp.body->SetType(b2_dynamicBody);
                comp.body->SetTransform(b2Vec2(0.0f, 0.0f), 0.0f);
                comp.body->SetBullet(true);   
        });

        bottom = engine.world.entity();
        bottom.set([&](spk::comp_tilebody_t& comp){
            for(uint32_t x = 0; x < comp.tilemap.size.x; x++) {
                comp.tilemap.tiles[x][0].id = (rand() % 2) + 1;
            }

            comp.add_fixtures();
            comp.body->SetTransform(b2Vec2(0.0f, -10.0f), 0.0f);
        });
    };

    auto end_ = [&](){
        sfk::log.log("ending sim");
        test.destruct();
        bottom.destruct();
    };

    engine.set_ppm(16.0f);

    engine.world.observer().event<spk::event_keyboard_t>().term<spk::tag_events_t>()
        .iter([&](flecs::iter& iter){
            auto event = iter.param<spk::event_keyboard_t>();
            auto camera = engine.get_current_camera();
        
            if(event->type == SDL_KEYDOWN || event->repeat) {
                switch(event->keysym.scancode) {
                case SDL_SCANCODE_A:
                    engine.set_current_camera(cam1);
                    sfk::log.log("A key");
                    break;
                case SDL_SCANCODE_D: 
                    engine.set_current_camera(cam2);
                    sfk::log.log("D key"); 
                    break;
                case SDL_SCANCODE_S: 
                    test.add<spk::comp_primitive_render_t>();
                    sfk::log.log("S key");
                    break;
                case SDL_SCANCODE_W:
                    test.remove<spk::comp_primitive_render_t>();
                    sfk::log.log("W key");
                    break;
                }
            }

            engine.get_current_camera()->recalculate();
        });

    engine.world.observer().event<spk::event_mouse_wheel_t>().term<spk::tag_events_t>()
        .iter([&](flecs::iter& iter){
            auto event = iter.param<spk::event_mouse_wheel_t>();
            float step = 0.1f;

            if(event->y < 0) {
                engine.set_ppm(engine.get_ppm() - step);
            } else if(event->y > 0) {
                engine.set_ppm(engine.get_ppm() + step);
            }
        });

    engine.world.observer().event<spk::event_window_mouse_click_t>().term<spk::tag_events_t>()
        .iter([&](flecs::iter& iter){
            auto event = iter.param<spk::event_window_mouse_click_t>();
            auto box_world = engine.get_current_b2World();
            auto camera = engine.get_current_camera();

            glm::vec2 mouse_click = camera->get_world_position((glm::vec2){(float)event->x, (float)event->y});
          
            if(test.is_alive() && test.has<spk::comp_tilebody_t>()) {
                test.set([&](spk::comp_tilebody_t& body) {
                    b2Vec2 position = body.body->GetPosition();
                    
                    b2Vec2 dir = sfk::to_box_vec2(mouse_click) - position;
                    dir.Normalize();
                    dir *= 200.0f;

                    body.body->SetTransform(sfk::to_box_vec2(mouse_click), 0.0f);
                    body.body->SetAwake(true);
                });
            }
        });

    engine.user_state.tick = 
        [&](spk::engine_t& engine){
            switch(my_state) {
            case STATE_LOAD:
                play_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_play_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
            
                start_();
                sfk::log.rule_map["myrule"] = "[yellow] goo";
                sfk::log.log("[it, em, myrule]This is video game[reset]");
                my_state = STATE_PLAY;
                
                sfk::log.log("loading state");
                break;

            case STATE_PLAY: {
            } break;

            case STATE_EXIT_PLAY:
                play_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_play_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;

                my_state = STATE_MENU;

                end_();

                sfk::log.log("exiting play");
                break;

            case STATE_EXIT:
                engine.exit(0);
                sfk::log.log("exiting app");
                break;

            default:
                break;
            }
        };

    exit_code = engine.run();
    engine.free();

    return exit_code;
}