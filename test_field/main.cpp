#include "engine.hpp"

enum current_state_e: uint32_t {
    STATE_MENU,
    STATE_LOAD,
    STATE_PLAY,
    STATE_EXIT_PLAY,
    STATE_EXIT
};

void exit_game(spk::scene_t& scene, spk::ui_button_t* button) {
    scene.user_game_state = STATE_EXIT;
}

void play_game(spk::scene_t& scene, spk::ui_button_t* button) {
    scene.user_game_state = STATE_LOAD;
}

void exit_play_game(spk::scene_t& scene, spk::ui_button_t* button) {
    scene.user_game_state = STATE_EXIT_PLAY;
}

class game_t : public spk::system_t {
public:
    void init_menu(spk::ui_canvas_t& canvas) {
        exit_btn = canvas.btns.malloc();
        exit_btn->flags = spk::UI_ELEMENT_FLAGS_ENABLED | spk::UI_ELEMENT_FLAGS_RELATIVE;
        exit_btn->pos = { -0.4f, -0.8f };
        exit_btn->size = { -0.5f, -0.9f };
        exit_btn->callback = exit_game;
        canvas.add_child(exit_btn);

        play_btn  = canvas.btns.malloc();
        play_btn->flags = spk::UI_ELEMENT_FLAGS_ENABLED | spk::UI_ELEMENT_FLAGS_RELATIVE;
        play_btn->pos = { -0.4f, -0.55f };
        play_btn->size = { -0.5f, -0.9f };
        play_btn->callback = play_game;
        canvas.add_child(play_btn);

        exit_play_btn = canvas.btns.malloc();
        exit_play_btn->flags = spk::UI_ELEMENT_FLAGS_RELATIVE;
        exit_play_btn->pos  = { -0.4f, -0.8f };
        exit_play_btn->size = { -0.5f, -0.9f };
        exit_play_btn->callback = exit_play_game;
        canvas.add_child(exit_play_btn);

        spk::ui_text_t* txt = canvas.texts.malloc();
        txt->flags = spk::UI_ELEMENT_FLAGS_ENABLED | spk::UI_ELEMENT_FLAGS_PARENT_RELATIVE;
        txt->pos = { 0.0f, 0.0f };
        txt->text.set("exit", 1.0f, {0.0f, 0.0f, 0.0f});
        exit_btn->add_child(txt);

        spk::ui_text_t* txt2 = canvas.texts.malloc();
        txt2->flags = spk::UI_ELEMENT_FLAGS_ENABLED | spk::UI_ELEMENT_FLAGS_PARENT_RELATIVE;
        txt2->pos = { 0.0f, 0.0f };
        txt2->text.set("play", 1.0f, {0.0f, 0.0f, 0.0f});
        play_btn->add_child(txt2);

        spk::ui_text_t* exit_play_text = canvas.texts.malloc();
        exit_play_text->flags = spk::UI_ELEMENT_FLAGS_ENABLED | spk::UI_ELEMENT_FLAGS_PARENT_RELATIVE;
        exit_play_text->pos = { 0.0f, 0.0f };
        exit_play_text->text.set("exit to menu", 1.0f, {0.0f, 0.0f, 0.0f});
        exit_play_btn->add_child(exit_play_text);
    }

    void start(spk::scene_t& scene) {
        b2World* world = scene.physics_scene->world;

        ball = scene.world.entity();
        ball.add<spk::primitive_render_t>();
        ball.add<spk::comp_b2Body>().set([&](spk::comp_b2Body& body){
            
            b2BodyDef body_def;
            body_def.angle = 0.0f;
            body_def.enabled = true;
            body_def.position = b2Vec2(0.0f, 0.0f);
            body_def.type = b2BodyType::b2_dynamicBody;
            body.body = world->CreateBody(&body_def);

            b2CircleShape circle;
            circle.m_radius = 1.0f;

            spk::add_body_fixture(&body, &circle, 0.2f, 0.0f, 1.0f, 1.0f);
        });
    }

    void end(spk::scene_t& scene) {
        ball.destruct();
    }

    void init(spk::scene_t& scene, void* extradata) {
        spk::ui_canvas_t& canvas = scene.canvas; 
        b2World* world = scene.physics_scene->world;

        init_menu(canvas);
        scene.user_game_state = STATE_MENU;
        
        scene.physics_scene->world->SetGravity(b2Vec2(0.0f, -9.81f));
    }

    void tick(spk::scene_t& scene, float delatime) {
        switch(scene.user_game_state) {
        case STATE_LOAD:
            play_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;
            exit_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;
            exit_play_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
        
            start(scene); // start the simulation

            scene.user_game_state = STATE_PLAY;
            break;

        case STATE_PLAY:
            // play_pong
            break;

        case STATE_EXIT_PLAY:
            play_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
            exit_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
            exit_play_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;

            end(scene); // end the simulation

            scene.user_game_state = STATE_MENU;
            break;

        case STATE_EXIT:
            scene.engine->set_time_exit(0);
            break;

        default:
            break;
        }
    }

    void update(spk::scene_t& scene, float deltatime) {
    }

    void free(spk::scene_t& scene) {

    }

private:
    spk::ui_button_t* exit_btn;
    spk::ui_button_t* play_btn;
    spk::ui_button_t* exit_play_btn;
    current_state_e state;
    flecs::entity ball;
};

int main() {
    spk::engine_t engine;
    game_t game;

    engine.init(800, 800, "pong game");
    engine.push_system(&game);

    spk::font_t* font = engine.resource_manager.load_ascii_font("generic.otf", 0, 32);
    if(!font) {
        engine.free();
        return -1; 
    }

    engine.loop();
    engine.free();
    return 0;
}