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
    
    engine.init();

    { // engine and window setup
        engine.set_target_fps(10000);
        engine.set_vsync_opt(spk::VSYNC_ENABLED);
        engine.set_current_window_title("hello world");
        engine.set_current_window_size(700, 700);

        b2World* world = engine.get_current_b2World();
        world->SetGravity(b2Vec2(0.0f, 0.0f));
    }

    // resource management 
    {
        engine.rsrc_mng.font_load_ascii("./Raleway-Regular.ttf");
        engine.rsrc_mng.atlas_init(0, 16, 16);
        if(!engine.rsrc_mng.atlas_load_from_path(0, "./test_atlas2.png")) {
            sfk::log.log("failed to load atlas");
            return 4;
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
        td[2].density = 1000.0f;
    }   

    auto start_ = [&]() -> void {
        for(uint32_t i = 0; i < 10; i++) {
            test = engine.world.entity();
            test.set([&](spk::comp_tilebody_t& comp){
                uint32_t id = (rand() % 2) + 1;

                comp.tilemap.tiles[0][0].id = id;
                comp.tilemap.tiles[1][0].id = id;
                comp.tilemap.tiles[2][0].id = id;

                comp.tilemap.tiles[0][1].id = id; 
                comp.tilemap.tiles[2][1].id = id; 

                comp.tilemap.tiles[0][2].id = id; 
                comp.tilemap.tiles[1][2].id = id; 
                comp.tilemap.tiles[2][2].id = id; 
                
                comp.add_fixtures();
                comp.body->SetType(b2_dynamicBody);
                comp.body->SetTransform(b2Vec2((float)i * 4.0f, 0.0f), 0.0f);
                comp.body->SetBullet(true);
            });
        }

        bottom = engine.world.entity();
        bottom.set([&](spk::comp_tilebody_t& comp){
            for(uint32_t x = 0; x < comp.tilemap.size.x; x++) {
                comp.tilemap.tiles[x][0].id = (rand() % 2) + 1;
            }

            comp.body->SetTransform(b2Vec2(0.0f, -10.0f), 0.0f);
            comp.add_fixtures();
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
            auto camera = engine.get_state().get_current_renderer().get_ref<spk::comp_camera_t>();
        
            if(event->type == SDL_KEYDOWN || event->repeat) {
                switch(event->keysym.scancode) {
                case SDL_SCANCODE_A:
                    camera->pos.x -= 10.0f;
                    sfk::log.log("A key");
                    break;
                case SDL_SCANCODE_D: 
                    camera->pos += 10.0f;
                    sfk::log.log("D key"); 
                    break;
                case SDL_SCANCODE_S: 
                    camera->pos.y += 10.0f;
                    sfk::log.log("S key");
                    break;
                case SDL_SCANCODE_W:
                    sfk::log.log("W key");
                    camera->pos.y -= 10.0f;
                    break;
                }
            }

            camera->recalculate();
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

            glm::ivec2 window_size = engine.get_current_window_size();
            // perspective offset
            float perspx = 0.0f, perspy = 0.0f;

            float rel_window_width  = (window_size.x / engine.get_ppm()) / 2.0f;
            float rel_window_height = (window_size.y / engine.get_ppm()) / 2.0f;

            float mouse_y = window_size.y - event->y; // flip the y

            float mouse_clickx = perspx + ((float)event->x / engine.get_ppm() - rel_window_width);
            float mouse_clicky = perspy + ((float)mouse_y  / engine.get_ppm() - rel_window_height);

            if(test.is_alive() && test.has<spk::comp_tilebody_t>()) {
                test.set([&](spk::comp_tilebody_t& body) {
                    b2Vec2 position = body.body->GetPosition();
                    
                    b2Vec2 dir = b2Vec2(mouse_clickx, mouse_clicky) - position;
                    dir.Normalize();
                    dir *= 2000.0f;

                    body.body->ApplyLinearImpulseToCenter(dir, true);
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