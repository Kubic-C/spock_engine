#include "engine.hpp"

/* listed example 001 for spock engine 0.0.0 */

enum wall_direction_e {
    LEFT = 0,
    RIGHT = 1
};

struct wall_tt {
    bool won = false;
    flecs::entity e;
};

void exit_btn_callback(spk::scene_tt& scene, spk::ui_button_tt* btn) {
    scene.engine->set_time_exit(0);
}

class contact_listener : public b2ContactListener {
    void BeginContact(b2Contact* contact) {
        flecs::entity* ball = nullptr;
        wall_tt* wall = nullptr;

        if(contact->GetFixtureA()->IsSensor()) {
            wall = (wall_tt*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        } else {
            ball = (flecs::entity*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        }

        if(contact->GetFixtureB()->IsSensor()) {
            wall = (wall_tt*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
        } else {
            ball = (flecs::entity*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        }

        if(wall == nullptr) {
            return;
        }

        wall->won = true;
    }

    void EndContact(b2Contact* contact) {
        
    }
};

#define LEFT_PADDLE_X -15.0f
#define RIGHT_PADDLE_X 15.0f
#define SCORE_MAX 4

class game_tt : public spk::system_tt {
public:
    void init(spk::scene_tt& scene, void* data) {
        spk::engine_tt* p_engine = (spk::engine_tt*)data;
        flecs::world& world = p_engine->scene->world;
        b2World* phy_world = scene.physics_scene->world; 

        srand(time(0));
        name = "Game";

        phy_world->SetGravity(b2Vec2_zero);
        phy_world->SetContactListener(&cl);

        left_paddle = world.entity();
        left_paddle.add<spk::primitive_render_tt>();
        left_paddle.add<spk::comp_b2Body>().set([&](spk::comp_b2Body& comp){
            b2Body*& body = comp.body;

            b2BodyDef body_def;
            body_def.position = { LEFT_PADDLE_X, 0.0f };
            body_def.type = b2_dynamicBody;
            body_def.fixedRotation = true;
            body = phy_world->CreateBody(&body_def);

            b2PolygonShape shape_def;
            shape_def.SetAsBox(0.5f, 5.0f);

            b2FixtureDef fixture_def;
            fixture_def.shape = &shape_def;
            fixture_def.density = 1.0f;
            fixture_def.restitution = 1.0f;
            body->CreateFixture(&fixture_def);
        });

        right_paddle = world.entity();
        right_paddle.add<spk::primitive_render_tt>();
        right_paddle.add<spk::comp_b2Body>().set([&](spk::comp_b2Body& comp){
            b2Body*& body = comp.body;

            b2BodyDef body_def;
            body_def.position = { RIGHT_PADDLE_X, 0.0f };
            body_def.type = b2_dynamicBody;
            body_def.fixedRotation = true;
            body = phy_world->CreateBody(&body_def);

            b2PolygonShape shape_def;
            shape_def.SetAsBox(0.5f, 5.0f);

            b2FixtureDef fixture_def;
            fixture_def.shape = &shape_def;
            fixture_def.density = 1.0f;
            fixture_def.restitution = 1.0f;
            body->CreateFixture(&fixture_def);
        });

        ball = world.entity();
        ball.add<spk::primitive_render_tt>();
        ball.add<spk::comp_b2Body>().set([&](spk::comp_b2Body& comp){
            b2Body*& body = comp.body;

            b2BodyDef body_def;
            body_def.position = { 0.0f, 0.0f };
            body_def.type = b2_dynamicBody;
            body_def.linearVelocity = b2Vec2(50.0f, 0.0f);
            body_def.bullet = true;
            body_def.fixedRotation = true;
            body_def.userData.pointer = (uintptr_t)&ball;
            body = phy_world->CreateBody(&body_def);

            b2CircleShape shape_def;
            shape_def.m_radius = 0.5f;
            shape_def.m_p = (b2Vec2){0.0f, 0.0f};

            b2FixtureDef fixture_def;
            fixture_def.shape = &shape_def;
            fixture_def.density = 1.0f;
            fixture_def.restitution = 1.2f;
            body->CreateFixture(&fixture_def);
        });

        /* invisible bounds */
        top = world.entity();
        down = world.entity();
        left.e = world.entity();
        right.e = world.entity();

        top.add<spk::comp_b2Body>().set([&](spk::comp_b2Body& comp){
            b2Body*& body = comp.body;

            b2BodyDef body_def;
            body_def.position = { 0.0f, 24.0f };
            body_def.type = b2_staticBody;
            body = phy_world->CreateBody(&body_def);

            b2PolygonShape shape_def;
            shape_def.SetAsBox(50.0f, 0.5f);

            b2FixtureDef fixture_def;
            fixture_def.shape = &shape_def;
            fixture_def.density = 1.0f;
            body->CreateFixture(&fixture_def);
        });
    
        down.add<spk::comp_b2Body>().set([&](spk::comp_b2Body& comp){
            b2Body*& body = comp.body;

            b2BodyDef body_def;
            body_def.position = { 0.0f, -24.0f };
            body_def.type = b2_staticBody;
            body = phy_world->CreateBody(&body_def);

            b2PolygonShape shape_def;
            shape_def.SetAsBox(50.0f, 0.5f);

            b2FixtureDef fixture_def;
            fixture_def.shape = &shape_def;
            fixture_def.density = 1.0f;
            body->CreateFixture(&fixture_def);
        });


        left.e.add<spk::comp_b2Body>().set([&](spk::comp_b2Body& comp){
            b2Body*& body = comp.body;

            b2BodyDef body_def;
            body_def.position = { -20.0f, 0 };
            body_def.type = b2_staticBody;
            body_def.userData.pointer = (uintptr_t)&left;
            body = phy_world->CreateBody(&body_def);

            b2PolygonShape shape_def;
            shape_def.SetAsBox(0.5f, 24.0f);

            b2FixtureDef fixture_def;
            fixture_def.shape = &shape_def;
            fixture_def.density = 1.0f;
            fixture_def.isSensor = true;
            body->CreateFixture(&fixture_def);
        });

        right.e.add<spk::comp_b2Body>().set([&](spk::comp_b2Body& comp){
            b2Body*& body = comp.body;

            b2BodyDef body_def;
            body_def.position = { 20.0f, 0 };
            body_def.type = b2_staticBody;
            body_def.userData.pointer = (uintptr_t)&right;
            body = phy_world->CreateBody(&body_def);

            b2PolygonShape shape_def;
            shape_def.SetAsBox(0.5f, 24.0f);

            b2FixtureDef fixture_def;
            fixture_def.shape = &shape_def;
            fixture_def.density = 1.0f;
            fixture_def.isSensor = true;
            body->CreateFixture(&fixture_def);
        });

        ltext = scene.canvas.texts.malloc();
        ltext->flags = spk::UI_ELEMENT_FLAGS_RELATIVE | spk::UI_ELEMENT_FLAGS_ENABLED;
        ltext->text = "0";
        ltext->pos = {-0.7f, 0.7f};
        scene.canvas.add_child((spk::ui_element_tt*)ltext);

        rtext = scene.canvas.texts.malloc();
        rtext->flags = spk::UI_ELEMENT_FLAGS_RELATIVE | spk::UI_ELEMENT_FLAGS_ENABLED;
        rtext->text = "0";
        rtext->pos = {0.7f, 0.7f};
        scene.canvas.add_child((spk::ui_element_tt*)rtext);

        spk::ui_button_tt* btn = scene.canvas.btns.malloc();
        btn->flags = spk::UI_ELEMENT_FLAGS_RELATIVE | spk::UI_ELEMENT_FLAGS_ENABLED;
        btn->pos = {-0.9f, -0.9f};
        btn->size = {-0.9f, -0.9f};
        btn->callback = exit_btn_callback;
        scene.canvas.add_child((spk::ui_element_tt*)btn);

        spk::ui_text_tt* txt = scene.canvas.texts.malloc();
        txt->flags = spk::UI_ELEMENT_FLAGS_PARENT_RELATIVE | spk::UI_ELEMENT_FLAGS_ENABLED;
        txt->text.color = {0.0f, 0.0f, 0.0f};
        txt->text = "exit";
        txt->pos = {0.0f, 0.0f};
        btn->add_child((spk::ui_element_tt*)txt);
    }

    void update(spk::scene_tt& scene, float deltatime) {
        left_paddle.set([&](spk::comp_b2Body& comp){ 
            comp.body->SetTransform(b2Vec2(LEFT_PADDLE_X, comp.body->GetPosition().y), 0.0f);
            comp.body->SetLinearVelocity(b2Vec2(0.0f, 0.0f)); 
        });
        right_paddle.set([&](spk::comp_b2Body& comp){ 
            comp.body->SetTransform(b2Vec2(RIGHT_PADDLE_X, comp.body->GetPosition().y), 0.0f);
            comp.body->SetLinearVelocity(b2Vec2(0.0f, 0.0f)); 
        });
    }

    void tick(spk::scene_tt& scene, float deltatime) {
        sfk::window_tt* window = scene.window;
        float speed = 1000.0f;

        if(game_end)
            return;

        if(lscore >= SCORE_MAX) {
            ltext->text.str.insert(0, "left paddle won: ");
            ltext->pos = {0.0f, 0.1f};
            scene.engine->set_time_exit(240); // exit in four seconds
            rtext->text = "";
            game_end = true;
            return;
        }

        if(rscore >= SCORE_MAX) {
            rtext->text.str.insert(0, "right paddle won: ");
            rtext->pos = {0.0f, 0.1f};
            scene.engine->set_time_exit(240); // exit in four seconds
            ltext->text = "";
            game_end = true;
            return;
        }

        if(window->get_key(GLFW_KEY_W) == GLFW_PRESS) {
            left_paddle.set([&](spk::comp_b2Body& comp){
                b2Body*& body = comp.body;

                body->SetLinearVelocity(b2Vec2(0.0f, speed * deltatime));
            });
        }
        if(window->get_key(GLFW_KEY_S) == GLFW_PRESS) {
            left_paddle.set([&](spk::comp_b2Body& comp){
                b2Body*& body = comp.body;

                body->SetLinearVelocity(b2Vec2(0.0f, -speed * deltatime));
            });
        }

        if(window->get_key(GLFW_KEY_UP) == GLFW_PRESS) {
            right_paddle.set([&](spk::comp_b2Body& comp){
                b2Body*& body = comp.body;

                body->SetLinearVelocity(b2Vec2(0.0f, speed * deltatime));
            });
        }
        if(window->get_key(GLFW_KEY_DOWN) == GLFW_PRESS) {
            right_paddle.set([&](spk::comp_b2Body& comp){
                b2Body*& body = comp.body;

                body->SetLinearVelocity(b2Vec2(0.0f, -speed * deltatime));
            });
        }

        if(left.won) {
            reset();
            rscore++;
            rtext->text = std::to_string(rscore);
            left.won = false;
        }

        if(right.won) {
            reset();
            lscore++;
            ltext->text = std::to_string(lscore);
            right.won = false;
        }
    }

    void free(spk::scene_tt& scene) {
    }

    void handle_message(spk::scene_tt& scene, float deltatime) {
    }

    void reset() {
        left_paddle.set([](spk::comp_b2Body& comp){
            comp.body->SetTransform(b2Vec2(LEFT_PADDLE_X, 0.0f), 0.0f);
        });

        right_paddle.set([](spk::comp_b2Body& comp){
            comp.body->SetTransform(b2Vec2(RIGHT_PADDLE_X, 0.0f), 0.0f);
        });

        ball.set([](spk::comp_b2Body& comp){
            comp.body->SetTransform(b2Vec2(0.0f, 0.0f), 45.0f);
            comp.body->SetLinearVelocity(b2Vec2(50.0f, 0.0f));
        });
    }

private:
    contact_listener cl;
    flecs::entity left_paddle, right_paddle;
    flecs::entity top, down;
    wall_tt left, right;
    flecs::entity ball;
    spk::ui_text_tt* ltext, *rtext;
    int lscore = 0, rscore = 0;
    bool game_end = false;
};

int main() {
    spk::engine_tt engine;
    game_tt game;

    engine.init(800, 800, "pong game");

    spk::font_tt* font = engine.resource_manager.load_ascii_font("generic.otf", 0, 32);
    if(!font) {
        engine.free();
        return -1; 
    }

    engine.push_system(&game);

    engine.loop();

    engine.free();
    return 0;
}