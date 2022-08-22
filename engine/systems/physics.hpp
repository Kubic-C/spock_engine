#pragma once
#include "../systems.hpp"
#include "../utility/physics_.hpp"

namespace spk {
    struct physics_scene_t {
        b2World* world; // physics world
    };

    class physics2D_t : public system_t {
    public:
        physics2D_t();
        void init(scene_t& scene, void* data);
        void update(scene_t& scene, float deltatime) {}
        void tick(scene_t& scene, float deltatime);
        void free(scene_t& scene);

    private:
    };
}
    
    