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
    engine.set_target_fps(10000);
    engine.set_vsync_opt(spk::VSYNC_ENABLED);


    b2World* world = engine.get_current_b2World();
    world->SetGravity(b2Vec2(0.0f, -100.0f));

    // test = engine.world.entity()
    //     .set([&](spk::comp_tilebody_t& tilebody){
    //         //tilebody.body->SetType(b2_dynamicBody);
    //         sfk::log.log("inside the code");
    //     });

    auto start_ = [&]() -> void {
        for(uint32_t i = 0; i < 20; i++) {
            test = engine.world.entity();
            test.add<spk::comp_primitive_render_t>();
            test.set([&](spk::comp_tilebody_t& comp){
                comp.dictionary[0].sprite.tax = 0;
                comp.dictionary[1].sprite.tax = 1;
                comp.dictionary[2].sprite.tax = 2;
                comp.dictionary[3].sprite.tax = 3;

                comp.tilemap.tiles[0][0].flags &= ~spk::TILE_FLAGS_EMPTY;
                comp.tilemap.tiles[1][0].flags &= ~spk::TILE_FLAGS_EMPTY;
                comp.tilemap.tiles[2][0].flags &= ~spk::TILE_FLAGS_EMPTY;

                comp.tilemap.tiles[0][1].flags &= ~spk::TILE_FLAGS_EMPTY;
                comp.tilemap.tiles[1][1].flags &= ~spk::TILE_FLAGS_EMPTY;
                comp.tilemap.tiles[2][1].flags &= ~spk::TILE_FLAGS_EMPTY;

                comp.tilemap.tiles[0][2].flags &= ~spk::TILE_FLAGS_EMPTY;
                comp.tilemap.tiles[1][2].flags &= ~spk::TILE_FLAGS_EMPTY;

                comp.add_fixtures();
                comp.body->SetType(b2_dynamicBody);
            });
        }

        bottom = engine.world.entity();
        bottom.set([&](spk::comp_tilebody_t& comp){
            comp.dictionary[0].sprite.tax = 0;
            comp.dictionary[1].sprite.tax = 1;
            comp.dictionary[2].sprite.tax = 2;
            comp.dictionary[3].sprite.tax = 3;

            for(uint32_t x = 0; x < comp.tilemap.size.x; x++) {
                comp.tilemap.tiles[x][0].id = 0;
                comp.tilemap.tiles[x][0].flags &= ~spk::TILE_FLAGS_EMPTY;
            }

            comp.body->SetTransform(b2Vec2(0.0f, -10.0f), 0.0f);
            comp.add_fixtures();
        });
    };

    sfk::log.log("body count %i", engine.get_current_b2World()->GetBodyCount());

    auto end_ = [&](){
        sfk::log.log("ending sim");
        test.destruct();
    };

    canvas_e = engine.get_state().get_current_canvas();
    auto canvas = canvas_e.get_ref<spk::ui_comp_canvas_t>();

    engine.rsrc_mng.atlas_init(0, 16, 16);
    if(!engine.rsrc_mng.atlas_load_from_path(0, "./test_atlas2.png")) {
        sfk::log.log("failed to load atlas");
        return 4;
    }

    engine.rsrc_mng.font_load_ascii("./Raleway-Regular.ttf");

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

    engine.set_ppm(5.0f);
    engine.world.observer().event<spk::event_window_mouse_wheel_t>().term<spk::tag_events_t>()
        .iter([&](flecs::iter& iter){
            auto event = iter.param<spk::event_window_mouse_wheel_t>();
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
                    body.body->SetTransform(b2Vec2(mouse_clickx, mouse_clicky), 0.0f);
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

    engine.set_current_window_title("hello world");
    engine.set_current_window_size(700, 700);

    exit_code = engine.run();

    sfk::log.log("exiting!");
    engine.free();

    return exit_code;
}