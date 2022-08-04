#include <iostream>
#include <string>
#include "engine.hpp"

void ball_collision_callback(flecs::entity e, spk::collision_info_tt& ci) {

    e.add<spk::body2D_tt>().set([](spk::body2D_tt& rigidbody){

        rigidbody.velocity.x = -(rigidbody.velocity.x * 5.0f);
    });
}

class game_tt : public spk::system_tt {
public:
    void init(spk::scene_tt& scene, void* data) {
        spk::engine_tt* p_engine = (spk::engine_tt*)data;
        flecs::world* world = &p_engine->scene->world;
        p_window = &p_engine->window;

        srand(time(0));

        {
            left_paddle = world->entity();
            left_paddle.add<spk::transform_tt>().set([](spk::transform_tt& transform){
                transform.position = {20.0f, 0.0f, 0.0f};
                transform.scale = {15.0f, 50.0f, 0.0f};
            });
            left_paddle.add<spk::aabb_tt>();
            left_paddle.add<spk::quad_render_tt>();
            left_paddle.add<spk::body2D_tt>().set([](spk::body2D_tt& rigidbody){
                rigidbody.velocity = { 0.0f, 0.0f };
                rigidbody.mass = 1000000.0f;
            });
        }
            
        {
            right_paddle = world->entity();
            right_paddle.add<spk::transform_tt>().set([](spk::transform_tt& transform){
                transform.position = {600.0f, 0.0f, 0.0f};
                transform.scale = {15.0f, 50.0f, 0.0f};
            });
            right_paddle.add<spk::aabb_tt>();
            right_paddle.add<spk::quad_render_tt>();
            right_paddle.add<spk::body2D_tt>().set([](spk::body2D_tt& rigidbody){
                rigidbody.velocity = { 0.0f, 0.0f };
            });
        }

        {
            ball = world->entity(1000UL);
            ball.add<spk::transform_tt>().set([](spk::transform_tt& transform){
                transform.position = {300.0f, 0.0f, 0.0f};
                transform.scale = {5.0f, 5.0f, 0.0f};
            });
            ball.add<spk::aabb_tt>();
            ball.add<spk::quad_render_tt>();
            ball.add<spk::body2D_tt>().set([](spk::body2D_tt& rigidbody){
                rigidbody.velocity = { -50.0f, 1.0f };
                rigidbody.collision_callback = &ball_collision_callback;
            });
        }

        flecs::entity e = world->entity();
        e.add<spk::transform_tt>().set([&](spk::transform_tt& transform){
            transform.position = {0.0f, 0.0, 0.0f};
            transform.scale = {20.0f, 20.0f, 0.0f};
        });
        e.add<spk::aabb_tt>();
        e.add<spk::quad_render_tt>();
        e.add<spk::body2D_tt>().set([](spk::body2D_tt& rigidbody){
            rigidbody.velocity = { 10.0f, 0.0f };
        });

        for(uint32_t i = 0; i < 100; i++) {
            flecs::entity e = world->entity();
            e.add<spk::transform_tt>().set([&](spk::transform_tt& transform){
                transform.position = {5.0f + ((float)i * 5.0f), 0.0, 0.0f};
                transform.scale = {5.0f, 5.0f, 0.0f};
            });
            e.add<spk::aabb_tt>();
            e.add<spk::quad_render_tt>();
            e.add<spk::body2D_tt>().set([](spk::body2D_tt& rigidbody){
                rigidbody.velocity = { 6.0f, 0.0f };
            });
        }


        name = "Game";
    }

    void update(spk::scene_tt& scene, float deltatime) {
        float speed = 100.0f;

        left_paddle.set([&](spk::body2D_tt& transform){ 
            if(scene.window->get_key(GLFW_KEY_W) & GLFW_PRESS) {
                transform.velocity.y += speed * deltatime;
            }
            if(scene.window->get_key(GLFW_KEY_S) & GLFW_PRESS) {
                transform.velocity.y -= speed * deltatime;
            }
        });

        right_paddle.set([&](spk::body2D_tt& transform){ 
            if(scene.window->get_key(GLFW_KEY_UP) & GLFW_PRESS) {
                transform.velocity.y += speed * deltatime;
            }
            if(scene.window->get_key(GLFW_KEY_DOWN) & GLFW_PRESS) {
                transform.velocity.y -= speed * deltatime;
            }
        });
    }

    void tick(spk::scene_tt& scene, float deltatime) {
        
    }
   
    void free() {
    }

    void handle_message(spk::scene_tt& scene, float deltatime, spk::message_tt& msg) {
    }

private:
    sfk::window_tt* p_window;

    flecs::entity left_paddle;
    flecs::entity right_paddle;
    flecs::entity ball;
};

int main() {
    spk::engine_tt engine;
    game_tt game;

    enum values { min = 0, max = 1 };

    engine.init(620, 800, "Game");
    engine.push_system(&game);

    engine.loop();

    engine.free();
    return 0;
}