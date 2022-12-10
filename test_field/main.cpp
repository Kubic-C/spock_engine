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

struct block_t {
    b2Fixture* fixture = nullptr;
    uint32_t amount = 0; // how many blocks ahead contain the same amount of fixtures
};

std::array<block_t, 256> row;

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
    flecs::entity test;

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

        test = engine.world.entity();
        test.add<spk::comp_primitive_render_t>();
        test.set([&](spk::comp_b2Body_t& body){
            b2BodyDef body_def;
            body_def.angle = 0.0f;
            body_def.enabled = true;
            body_def.position = b2Vec2(0.0f, 0.0f);
            body_def.type = b2BodyType::b2_dynamicBody;
            body.body = world->CreateBody(&body_def);
            
            b2PolygonShape shape;
            shape.SetAsBox(1, 1);
            shape.m_vertices[0].x += 5 * 2; // must include half width
            shape.m_vertices[1].x += 5 * 2; // |
            shape.m_vertices[2].x += 5 * 2; // |
            shape.m_vertices[3].x += 5 * 2; // |

            b2FixtureDef def;
            def.density = 1.0f;
            def.shape = &shape;

            row[5].fixture = body.body->CreateFixture(&def);
        });
    };

    auto end_ = [&](){
        sfk::log.log("ending sim");
        ball.destruct();
        bottom.destruct();
        test.destruct();
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

    double last_second = 0.0;
    double last_10_seconds = 0.0;
    uint32_t x = 0;
    uint32_t cur_i = 0;

    engine.world.system().kind(flecs::OnUpdate).interval(engine.get_state()._get_target_tps())
        .iter([&](flecs::iter& iter){
            switch(my_state) {
            case STATE_LOAD:
                play_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_play_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
            
                start_(); // start the simulation

                my_state = STATE_PLAY;
                
                sfk::log.log("loading state");
                break;

            case STATE_PLAY: {

                // proof of concept, combining meshes for tilemaps
                if(last_second + 1 < sfk::time.get_time()) {
                    auto body = test.get<spk::comp_b2Body_t>()->body;
                    
                    b2PolygonShape shape;
                    shape.SetAsBox(1, 1);
                    shape.m_vertices[0].x += x; // must include half width
                    shape.m_vertices[1].x += x; // |
                    shape.m_vertices[2].x += x; // |
                    shape.m_vertices[3].x += x; // |

                    b2FixtureDef def;
                    def.density = 1.0f;
                    def.shape = &shape;

                    row[cur_i].fixture = body->CreateFixture(&def);

                    for(uint32_t i = 0; i != 256; i += 2) {
                        if(!row[i].fixture)
                            continue;

                        b2Fixture* fixture = row[i].fixture;
                        uint32_t start_next_i = i + (row[i].amount + 1);
                        uint32_t end_next_i = row[start_next_i].amount + start_next_i;
                        b2Fixture* next_fixture = row[start_next_i].fixture;

                        if(!next_fixture)
                            continue;

                        b2PolygonShape* cur_shape = static_cast<b2PolygonShape*>(fixture->GetShape());
                        b2PolygonShape* next_shape = static_cast<b2PolygonShape*>(next_fixture->GetShape());

                        sfk::log.log("[cur] %f, [nxt] %f", cur_shape->m_vertices[1].x, next_shape->m_vertices[0].x);
                        if(cur_shape->m_vertices[1].x == next_shape->m_vertices[0].x) {
                            // combine the two fixtues
                            b2Vec2 points[4] = {
                                cur_shape->m_vertices[0],
                                next_shape->m_vertices[1],
                                next_shape->m_vertices[2],
                                cur_shape->m_vertices[3]

                            };
                            memcpy(cur_shape->m_vertices, points, sizeof(b2Vec2)*4);
                            body->DestroyFixture(next_fixture);

                            // starting from the begginning, set all the rows 
                            // that contained the destroyed fixture to the new fixture
                            // and the new amount relative to the end of next amount
                            for(uint32_t j = i; j != end_next_i; j++) {
                                row[j].fixture = fixture;
                                row[j].amount = end_next_i - i;
                            }

                            // start from the beggining again, memory locations
                            // will certainly move after deleting a fixture
                            i = 0;
                        }
                    }

                    x += 4;
                    cur_i += 2;
                    last_second = sfk::time.get_time();

                }
            } break;

            case STATE_EXIT_PLAY:
                play_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_btn->flags |= spk::UI_ELEMENT_FLAGS_ENABLED;
                exit_play_btn->flags &= ~spk::UI_ELEMENT_FLAGS_ENABLED;

                end_(); // end the simulation

                my_state = STATE_MENU;

                sfk::log.log("exiting play");
                break;

            case STATE_EXIT:
                engine.get_state().exit(0);
                sfk::log.log("exiting app");
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