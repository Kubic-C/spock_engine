#include "physics.hpp"
#include "../engine.hpp"

namespace spk {
    physics2D_tt::physics2D_tt() {

    }

    void physics2D_tt::init(scene_tt& scene, void* data) {
        flecs::world& world = scene.world;
        b2World* physics_world = new b2World(b2Vec2(0.0f, -9.81f));
        physics_world->SetAllowSleeping(false);
        
        scene.physics_scene->world = physics_world;
    
        add_box2d_components(world, physics_world);
    }
    
    void physics2D_tt::tick(scene_tt& scene, float deltatime) {
        flecs::world& world = scene.world;

        scene.physics_scene->world->Step(scene.engine->get_tick_speed(), 8, 3);        
    }

    void physics2D_tt::free(scene_tt& scene) {
        delete scene.physics_scene->world;
    }
}