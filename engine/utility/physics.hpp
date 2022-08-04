#pragma

#include "aabb.hpp"

/*
    physics utility
*/

#define SFK_QUAD_REGION_BRANCH -1

namespace spk {
    struct body2D_tt;
    struct collision_info_tt;

    typedef void(*collision_callback_tt)(flecs::entity e, collision_info_tt& ci);

    struct body2D_tt {
        float restitution;
        float mass;
        glm::vec2 velocity;
        bool kinematic; // not affected by gravity
        float gravity;        
        collision_callback_tt collision_callback; // collision callback

        body2D_tt() {
            sfk::zero(this);
            kinematic = true;
            gravity = 0.01f;
            velocity = {};
            mass = 1.0f;
            restitution = 1.0f;
        }
    };

    struct quad_region_tt {
        int32_t children;

        int32_t count;
    };

    struct quad_tree_tt {
        int32_t children;
    };

    struct collider_comp_tt {
        transform_tt* transform;
        aabb_tt* aabb;
        body2D_tt* body;
    };

    struct collision_info_tt {
        bool collision; // true if a collision happened
        collider_comp_tt one, two;
        glm::vec2 cn;
        glm::vec2 cp;
        float ct;
    };

    bool aabb_vs_aabb(collision_info_tt* colliders, rect_tt rect[2] = nullptr);

    void collision_resolution(collision_info_tt* colliders, rect_tt rect[2], float deltatime);

    bool ray_vs_aabb(const glm::vec2& org, const glm::vec2& dir, const rect_tt& rect, float& t_hit_near, 
        glm::vec2& contact_point, glm::vec2& contact_normal);
}