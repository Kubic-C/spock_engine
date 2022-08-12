#include <iostream>
#include <string>
#include "engine.hpp"

class game_tt : public spk::system_tt {
public:
    void init(spk::scene_tt& scene, void* data) {
        spk::engine_tt* p_engine = (spk::engine_tt*)data;
        flecs::world& world = p_engine->scene->world;
        b2World* phy_world = scene.physics_scene->world; 

        scene.canvas.font = 0;
        srand(time(0));
        name = "Game";

        phy_world->SetGravity(b2Vec2_zero);
        
        left_paddle = world.entity();
        left_paddle.add<spk::primitive_render_tt>();
        left_paddle.add<spk::comp_b2Body>().set([&](spk::comp_b2Body& comp){
            b2Body*& body = comp.body;

            b2BodyDef body_def;
            body_def.position = { -15.0f, 0.0f };
            body_def.type = b2_kinematicBody;
            body = phy_world->CreateBody(&body_def);

            b2PolygonShape shape_def;
            shape_def.SetAsBox(0.5f, 5.0f);

            b2FixtureDef fixture_def;
            fixture_def.shape = &shape_def;
            fixture_def.density = 1.0f;
            body->CreateFixture(&fixture_def);
        });

        right_paddle = world.entity();
        right_paddle.add<spk::primitive_render_tt>();
        right_paddle.add<spk::comp_b2Body>().set([&](spk::comp_b2Body& comp){
            b2Body*& body = comp.body;

            b2BodyDef body_def;
            body_def.position = { 15.0f, 0.0f };
            body_def.type = b2_kinematicBody;
            body = phy_world->CreateBody(&body_def);

            b2PolygonShape shape_def;
            shape_def.SetAsBox(0.5f, 5.0f);

            b2FixtureDef fixture_def;
            fixture_def.shape = &shape_def;
            fixture_def.density = 1.0f;
            body->CreateFixture(&fixture_def);
        });

        ball = world.entity();
        ball.add<spk::primitive_render_tt>();
        ball.add<spk::comp_b2Body>().set([&](spk::comp_b2Body& comp){
            b2Body*& body = comp.body;

            b2BodyDef body_def;
            body_def.position = { 0.0f, 0.0f };
            body_def.type = b2_dynamicBody;
            body_def.linearVelocity = b2Vec2(20, 0.0f);
            body_def.bullet = true;
            body = phy_world->CreateBody(&body_def);

            b2PolygonShape shape_def;
            shape_def.SetAsBox(0.5f, 0.5f);

            b2FixtureDef fixture_def;
            fixture_def.shape = &shape_def;
            fixture_def.density = 1.0f;
            fixture_def.restitution = 1.5f;
            body->CreateFixture(&fixture_def);
        });

        /* invisible bounds */
        top = world.entity();
        down = world.entity();

        top.add<spk::primitive_render_tt>();
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
            fixture_def.restitution = 1.5f;
            body->CreateFixture(&fixture_def);
        });
    
        down.add<spk::primitive_render_tt>();
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
            fixture_def.restitution = 1.5f;
            body->CreateFixture(&fixture_def);
        });
    }

    void update(spk::scene_tt& scene, float deltatime) {
        sfk::window_tt* window = scene.window;
        float speed = 1000.0f;

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
    }

    void tick(spk::scene_tt& scene, float deltatime) {

    }
   
    void free(spk::scene_tt& scene) {
    }

    void handle_message(spk::scene_tt& scene, float deltatime) {
    }

private:
    flecs::entity left_paddle, right_paddle;
    flecs::entity top, down;
    flecs::entity ball;
};

int main() {
    spk::engine_tt engine;
    game_tt game;

    engine.init(400, 400, "Game");
    engine.push_system(&game);

    if(!engine.resource_manager.load_ascii_font("generic.otf", 0, 32)) {
        engine.free();
        return -1; 
    }

    engine.loop();

    engine.free();
    return 0;
}