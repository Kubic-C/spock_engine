#include "spock.hpp"

enum current_state_e: uint32_t {
    STATE_MENU,
    STATE_LOAD,
    STATE_PLAY,
    STATE_EXIT_PLAY,
    STATE_EXIT
};

void exit_game(spk::engine_t& engine, spk::ui_button_t& button) {
    *(current_state_e*)engine.user_state = STATE_EXIT;
}

void play_game(spk::engine_t& engine, spk::ui_button_t& button) {
    *(current_state_e*)engine.user_state = STATE_LOAD;
}

void exit_play_game(spk::engine_t& engine, spk::ui_button_t& button) {
    *(current_state_e*)engine.user_state = STATE_EXIT_PLAY;
}

MAIN {
    int exit_code = 0;
    spk::engine_t engine;
    flecs::entity e;
    flecs::entity canvas_e;
    spk::ui_button_t* exit_btn;
    spk::ui_button_t* play_btn;
    spk::ui_button_t* exit_play_btn;
    current_state_e my_state;
    flecs::entity ball;
    flecs::entity bottom;


    engine.init();
    engine.set_target_fps(10000);
    engine.set_vsync_opt(spk::VSYNC_ENABLED);

    auto start_ = [&](){
        sfk::log.log("starting sim");
        b2World* world = engine.get_current_b2World();

        ball = engine.world.entity();
        ball.add<spk::comp_primitive_render_t>();
        ball.add<spk::comp_b2Body_t>().set([&](spk::comp_b2Body_t& body){
            
            b2BodyDef body_def;
            body_def.angle = 0.0f;
            body_def.enabled = true;
            body_def.position = b2Vec2(-5.0f, 5.0f);
            body_def.type = b2BodyType::b2_dynamicBody;
            body.body = world->CreateBody(&body_def);

            b2CircleShape circle;
            circle.m_radius = 1.0f;

            spk::add_body_fixture(&body, &circle, 0.2f, 1.0f, 1.0f, 1.0f);
        });

        bottom = engine.world.entity();
        bottom.add<spk::comp_primitive_render_t>();
        bottom.add<spk::comp_b2Body_t>().set([&](spk::comp_b2Body_t& body){
            b2BodyDef body_def;
            body_def.angle = 0.0f;
            body_def.enabled = true;
            body_def.position = b2Vec2(0.0f, -20.0f);
            body_def.type = b2BodyType::b2_staticBody;
            body.body = world->CreateBody(&body_def);

            b2PolygonShape shape;
            shape.SetAsBox(100.0f, 5.0f);

            spk::add_body_fixture(&body, &shape, 0.2f, 0.0f, 1.0f, 1.0f);
        });
    };

    auto end_ = [&](){
        sfk::log.log("ending sim");
        ball.destruct();
        bottom.destruct();
    };

    canvas_e = engine.get_state()._get_current_canvas();
    auto canvas = canvas_e.get_ref<spk::ui_comp_canvas_t>();
    engine.rsrc_mng.load_ascii_font("./LT_fun.otf");

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
    engine.user_state = &my_state;

    engine.world.system().kind(flecs::OnUpdate).interval(engine.get_state()._get_target_tps())
        .iter([&](flecs::iter& iter){
            switch(my_state) {
            case STATE_LOAD:
                play_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_play_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
            
                start_(); // start the simulation

                my_state = STATE_PLAY;

                break;

            case STATE_PLAY:
                // play_pong
                break;

            case STATE_EXIT_PLAY:
                play_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_play_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;

                end_(); // end the simulation

                my_state = STATE_MENU;
                break;

            case STATE_EXIT:
                engine.get_state().exit(0);
                break;

            default:
                break;
            }
        });

    engine.set_current_window_title("hello world");
    engine.set_current_window_size(700, 700);

    exit_code = engine.run();

    engine.free();

    return exit_code;
}