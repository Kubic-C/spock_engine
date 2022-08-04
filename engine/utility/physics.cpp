#include "physics.hpp"

namespace spk {
    bool aabb_vs_aabb(collision_info_tt* colliders, rect_tt rect[2]) {
        const rect_tt b1 = {
            { colliders->one.transform->position.x - colliders->one.aabb->half_width  * colliders->one.transform->scale.x,
              colliders->one.transform->position.y - colliders->one.aabb->half_height * colliders->one.transform->scale.y },
            { colliders->one.transform->position.x + colliders->one.aabb->half_width  * colliders->one.transform->scale.x,
              colliders->one.transform->position.y + colliders->one.aabb->half_height * colliders->one.transform->scale.y }
        };

        const rect_tt b2 = {
            { colliders->two.transform->position.x - colliders->two.aabb->half_width  * colliders->two.transform->scale.x,
              colliders->two.transform->position.y - colliders->two.aabb->half_height * colliders->two.transform->scale.y },
            { colliders->two.transform->position.x + colliders->two.aabb->half_width  * colliders->two.transform->scale.x,
              colliders->two.transform->position.y + colliders->two.aabb->half_height * colliders->two.transform->scale.y }
        };

        if(rect != null) {
            memcpy(rect[0], b1, sizeof(b1));
            memcpy(rect[1], b2, sizeof(b2));
        }

        if(b1[min][0] <= b2[max][0] &&
           b1[max][0] >= b2[min][0] &&
           b1[min][1] <= b2[max][1] &&
           b1[max][1] >= b2[min][1]) {

            ray_vs_aabb(colliders->one.transform->position, 
                        colliders->one.transform->position - colliders->two.transform->position, rect[0], 
                        colliders->ct, colliders->cp, colliders->cn);

            colliders->collision = true;
            return true;
        } else {
            colliders->collision = false;
            return false;
        }
    }

    void collision_resolution(collision_info_tt* colliders, rect_tt rect[2], float deltatime) {
        glm::vec2 offsethalf = (colliders->cn * (1.0f - colliders->ct)) / 2.0f;
        glm::vec2 _cn = colliders->cn;

        float e = std::min(colliders->one.body->restitution, colliders->two.body->restitution);

        glm::vec2 relative_velocity = colliders->one.body->velocity - colliders->two.body->velocity;
        float mag = (-(1 + e) * glm::dot(relative_velocity, colliders->cn)) 
            / ((1 / colliders->one.body->mass) + (1 / colliders->two.body->mass));


        glm::vec2 impulse = mag * colliders->cn;

        colliders->one.body->velocity += impulse;
        colliders->two.body->velocity += -impulse;

        colliders->one.transform->position += glm::vec3(colliders->one.body->velocity, 0.0f) * deltatime;
        colliders->two.transform->position += glm::vec3(colliders->two.body->velocity, 0.0f) * deltatime;
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