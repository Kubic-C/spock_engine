#include <iostream>
#include <string>
#include "engine.hpp"


class game_tt : public spk::system_tt {
public:
    void init(spk::scene_tt& scene, void* data) {
        spk::engine_tt* p_engine = (spk::engine_tt*)data;
        flecs::world& world = p_engine->scene->world;
        b2World* phy_world = scene.physics_scene->world; 
        p_window = &p_engine->window;

        srand(time(0));

        box = world.entity();
        box.add<spk::comp_b2Body>();
        box.add<spk::primitive_render_tt>().set([](spk::primitive_render_tt& render){
            render.color = (glm::vec3){1.0f, 0.0f, 0.2f};
        });

        box.set([&](spk::comp_b2Body& comp) {
            b2Body*& body = comp.body;
            b2BodyDef body_def;
            b2FixtureDef fix_def;
            b2PolygonShape shape;

            body_def.position.Set(0.0f, -10.0f);
            body_def.type = b2_dynamicBody;
            body = phy_world->CreateBody(&body_def);
            shape.SetAsBox(50.0f, 3.0f);

            fix_def.shape = &shape;
            fix_def.density = 1.0f;
            fix_def.friction = 0.90f;
            fix_def.restitution = 0.6f;
            body->CreateFixture(&fix_def); 
            fix_def.density = .1f;
            shape.SetAsBox(10.0f, 10.0f, b2Vec2(50.0f, 0.0f), 0.0f);
            body->CreateFixture(&fix_def); 

        });

        { // this is the platform below the BOX
            auto e = world.entity();
            e.add<spk::comp_b2Body>();
            e.add<spk::primitive_render_tt>().set([](spk::primitive_render_tt& render){
                render.color = (glm::vec3){1.0f, 0.0f, 0.2f};
            });

            e.set([&](spk::comp_b2Body& comp) {
                b2Body*& body = comp.body;
                b2BodyDef body_def;
                b2FixtureDef fix_def;
                b2PolygonShape shape;

                body_def.position.Set(0.0f, -20.0f);
                body_def.type = b2_staticBody;
                body = phy_world->CreateBody(&body_def);
                
                b2Vec2 list[] = {
                    {-1.0f, -1.0f },
                    { 1.0f, -1.0f },
                    { 0.0f,  60.0f }
                };

                shape.Set(list, 3);

                fix_def.shape = &shape;
                fix_def.density = 1.0f;
                fix_def.friction = 0.90f;
                fix_def.restitution = 0.6f;
                body->CreateFixture(&fix_def);
            });
        }
        

        srand(time(0));

        name = "Game";
    }

    void update(spk::scene_tt& scene, float deltatime) {
        float speed = 100.0f;
    }

    void tick(spk::scene_tt& scene, float deltatime) {
        box.set([](spk::comp_b2Body& comp) { // F(n) = m(kg) * a (m/s)
            b2PolygonShape* shape = (b2PolygonShape*)comp.body->GetFixtureList()->GetNext()->GetShape();
            comp.body->ApplyForce((comp.body->GetMass() * 10) * comp.body->GetWorldPoint(shape->m_normals[2]), b2Vec2(50.0f, 0), true);
        });
    }
   
    void free(spk::scene_tt& scene) {
    }

    void handle_message(spk::scene_tt& scene, float deltatime, spk::message_tt& msg) {
    }

private:
    sfk::window_tt* p_window;
    flecs::entity box;
};

int main() {
    spk::engine_tt engine;
    game_tt game;

    enum values { min = 0, max = 1 };

    engine.init(1000, 800, "Game");
    engine.push_system(&game);

    engine.loop();

    engine.free();
    return 0;
}