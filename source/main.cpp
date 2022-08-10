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


        scene.canvas.font = 0;

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
            fix_def.restitution = 0.0f;
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
                    { 0.0f,  1.0f }
                };

                shape.Set(list, 3);

                fix_def.shape = &shape;
                fix_def.density = 1.0f;
                fix_def.friction = 1.0f;
                fix_def.restitution = 0.0f;
                body->CreateFixture(&fix_def);
            });
        }

        srand(time(0));

        {
            const char* font_path = "./generic.otf";
            spk::font_manager_tt fmanager;
            if(!fmanager.init()) {
                printf("failed to init fmanager\n");
                abort();
            }

            if(!fmanager.load_ascii_font(32, 0, font_path)) {
                printf("failed to load font: %s\n", font_path);
                abort();
            }
        }

        name = "Game";
    }

    void update(spk::scene_tt& scene, float deltatime) {
        float speed = 100.0f;
    }

    void tick(spk::scene_tt& scene, float deltatime) {

    }
   
    void free(spk::scene_tt& scene) {
    }

    void handle_message(spk::scene_tt& scene, float deltatime) {
    }

private:
    uint32_t text_box = 1;
    sfk::window_tt* p_window;
    flecs::entity box;
};

int main() {
    spk::engine_tt engine;
    game_tt game;

    engine.init(1000, 800, "Game");
    engine.push_system(&game);

    if(!engine.resource_manager.load_ascii_font("generic.otf", 0, 32)) {
        engine.free();
        return -1; 
    }

    engine.loop();

    engine.free();
    return 0;
}