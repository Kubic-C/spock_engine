#pragma

#include "aabb.hpp"

/*
    physics utility
*/

#define SFK_QUAD_REGION_BRANCH -1

namespace spk {
    struct quad_region_tt {
        int32_t children;

        int32_t count;
    };

    struct quad_tree_tt {
        int32_t children;
    };

    struct collision_info_tt {
        glm::vec2 cn;
        glm::vec2 cp;
        float ct; // of the ray;
    };

    bool aabb_vs_aabb(transform_tt& transform1, aabb_tt& aabb1, transform_tt& transform2, aabb_tt& aabb2, rect_tt rect[2] = nullptr);

    void collision_resolution(transform_tt& transform1, transform_tt& transform2, rect_tt rect[2], collision_info_tt& ci);

    bool ray_vs_aabb(const glm::vec2& org, const glm::vec2& dir, const rect_tt& rect, float& t_hit_near, 
        glm::vec2& contact_point, glm::vec2& contact_normal);
}