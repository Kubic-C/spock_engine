#include <iostream>
#include <string>
#include "engine.hpp"

class game_tt : public spk::system_tt {
public:
    void init(spk::scene_tt& scene, void* data) {
        spk::engine_tt* p_engine = (spk::engine_tt*)data;
        flecs::world* world = &p_engine->scene->world;
        p_window = &p_engine->window;

        {
            auto e = world->entity();
            e = world->entity();
            e.add<spk::transform_tt>().set(
                spk::transform_tt{
                    .position = glm::vec3(200.0f, -250.0f, 0.0f),
                    .scale = glm::vec3(25.0f)
            });

            e.add<spk::rigidbody2D_tt>();
            e.add<spk::quad_render_tt>();
            e.add<spk::aabb_tt>();
        }

        {
            player = world->entity();
            player.add<spk::transform_tt>().set(
                spk::transform_tt{
                    .position = glm::vec3(200.0f, 0.0f, 0.0f),
                    .scale = glm::vec3(30.0f)
            });
            
            player.add<spk::rigidbody2D_tt>().set([](spk::rigidbody2D_tt& rigidbody) {
                rigidbody.kinematic = false;
                rigidbody.gravity = 1.0f;
            });

            player.add<spk::quad_render_tt>(); 
            player.add<spk::aabb_tt>();
        }

        name = "Game";
    }

    void update(spk::scene_tt& scene, float deltatime) {
    }

    void tick(spk::scene_tt& scene, float deltatime) {
        player.set([&](spk::transform_tt& transform) {
            if(p_window->get_key(GLFW_KEY_A))
                transform.position.x -= 100.0f * deltatime;
            if(p_window->get_key(GLFW_KEY_D))
                transform.position.x += 100.0f * deltatime;
        });
    }
   
    void free() {
    }

    void handle_message(spk::scene_tt& scene, float deltatime, spk::message_tt& msg) {
    }

private:
    flecs::entity player;
    sfk::window_tt* p_window;
};

int main() {
    spk::engine_tt engine;
    game_tt game;

    engine.init(425, 800, "Game");
    engine.push_system(&game);

    engine.loop();

    engine.free();
    return 0;
}