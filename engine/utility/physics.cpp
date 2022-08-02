#include "physics.hpp"

namespace spk {
    bool aabb_vs_aabb(transform_tt& transform1, aabb_tt& aabb1, transform_tt& transform2, aabb_tt& aabb2, rect_tt rect[2]) {
        const rect_tt b1 = {
            { transform1.position.x - aabb1.half_width  * transform1.scale.x,
              transform1.position.y - aabb1.half_height * transform1.scale.y },
            { transform1.position.x + aabb1.half_width  * transform1.scale.x,
              transform1.position.y + aabb1.half_height * transform1.scale.y }
        };

        const rect_tt b2 = {
            { transform2.position.x - aabb2.half_width  * transform2.scale.x,
              transform2.position.y - aabb2.half_height * transform2.scale.y },
            { transform2.position.x + aabb2.half_width  * transform2.scale.x,
              transform2.position.y + aabb2.half_height * transform2.scale.y }
        };

        if(rect != null) {
            memcpy(rect[0], b1, sizeof(b1));
            memcpy(rect[1], b2, sizeof(b2));
        }

        if(b1[min][0] <= b2[max][0] &&
           b1[max][0] >= b2[min][0] &&
           b1[min][1] <= b2[max][1] &&
           b1[max][1] >= b2[min][1]) {
            return true;
        } else {
            return false;
        }
    }

    void collision_resolution(transform_tt& transform1, transform_tt& transform2, rect_tt rect[2], collision_info_tt& ci) {
        glm::vec2 cp; // not used
        glm::vec2 cn;
        float ct;
        
        ray_vs_aabb(transform1.position, transform1.position - transform2.position, rect[0], ct, cp, cn);
        
        glm::vec2 offsethalf = (cn * (1.0f - ct)) / 2.0f;
        transform1.position += glm::vec3(-offsethalf, 0.0f);
        transform2.position += glm::vec3(offsethalf, 0.0f);

        ci.cn = cn; // youll need to negate this for _2 target (the other collider) 
        ci.cp = cp; 
        ci.ct = ct; 
    }

    bool ray_vs_aabb(const glm::vec2& org, const glm::vec2& dir, const rect_tt& rect, float& t_hit_near, 
                    glm::vec2& contact_point, glm::vec2& contact_normal) {
        glm::vec2 t_near = { (rect[min].x -  org.x) / dir.x, (rect[min].y - org.y) / dir.y};
        glm::vec2 t_far  = { (rect[max].x -  org.x) / dir.x, (rect[max].y - org.y) / dir.y};
    
        if(std::isnan(t_near.x) || std::isnan(t_near.y)) return false;
        if(std::isnan(t_far.x) || std::isnan(t_far.y)) return false;

        if(t_near.x > t_far.x)
            std::swap(t_near.x, t_far.x);
        if(t_near.y > t_far.y)
            std::swap(t_near.y, t_far.y);
    
        if(!(t_near.x < t_far.y || t_near.y < t_far.x)) 
            return false;

        t_hit_near = std::max(t_near.x, t_near.y);
        float t_hit_far = std::min(t_far.x, t_far.y);

        if(t_hit_far < 0)
            return false;

        contact_point = org + (dir * t_hit_near); 
		if (t_near.x > t_near.y)
			if (dir.x < 0)
				contact_normal = { 1, 0 };
			else
				contact_normal = { -1, 0 };
		else if (t_near.x < t_near.y)
			if (dir.y < 0)
				contact_normal = { 0, 1 };
			else
				contact_normal = { 0, -1 };

        return true;
    }
}