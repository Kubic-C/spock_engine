#include "physics.hpp"
#include "../engine.hpp"

namespace spk {
    physics2D_t::physics2D_t() {

    }

    void physics2D_t::init(scene_t& scene, void* data) {
        flecs::world& world = scene.world;
        b2World* physics_world = new b2World(b2Vec2(0.0f, -9.81f));
        physics_world->SetAllowSleeping(false);
        
        scene.physics_scene->world = physics_world;
    
        add_box2d_components(world, physics_world);
    }
    
    void physics2D_t::tick(scene_t& scene, float deltatime) {
        flecs::world& world = scene.world;

        scene.physics_scene->world->Step(scene.engine->get_tick_speed(), 8, 3);        
    }

    void physics2D_t::free(scene_t& scene) {
        delete scene.physics_scene->world;
    }
}