#pragma once
#include "../systems.hpp"
#include "../utility/physics.hpp"

namespace spk {
    struct physics_scene_tt {
        b2World* world; // physics world
    };

    class physics2D_tt : public system_tt {
    public:
        physics2D_tt();
        void init(scene_tt& scene, void* data);
        void update(scene_tt& scene, float deltatime) {}
        void tick(scene_tt& scene, float deltatime);
        void free(scene_tt& scene);

    private:
    };
}
    
    