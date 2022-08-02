#pragma once
#include "../systems.hpp"
#include "../utility/physics.hpp"

namespace spk {
    struct rigidbody2D_tt;

    typedef void(*collision_callback_tt)(flecs::entity e, collision_info_tt& ci);

    struct physics_scene_tt {

    };

    struct rigidbody2D_tt {
        glm::vec2 velocity;
        bool kinematic; // not affected by gravity
        float gravity;        
        collision_callback_tt collision_callback; // collision callback

        rigidbody2D_tt() {
            sfk::zero(this);
            kinematic = true;
            gravity = 0.01f;
            velocity = {};
        }
    };

    class physics2D_tt : public system_tt {
    public:
        physics2D_tt();
        void init(scene_tt& scene, void* data);
        void update(scene_tt& scene, float deltatime) {}
        void tick(scene_tt& scene, float deltatime);
        void free();

    private:
    };
}
    
    