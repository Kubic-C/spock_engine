#include "aabb.hpp"

namespace spk {
    aabb_t::aabb_t() {
        hw = 1.0f;
        hh = 1.0f;
    }

    aabb_t::aabb_t(float x, float y) {
        hw = x;
        hh = y;
    }

    aabb_t::aabb_t(const glm::vec2& other) {
        hw = other.x;
        hh = other.y;
    }

    bool overlap(const aabb_t& A_aabb, const glm::vec2& A_pos, 
                 const aabb_t& B_aabb, const glm::vec2& B_pos) {
        const float a_x_min = A_pos.x - A_aabb.hw; 
        const float a_x_max = A_pos.x + A_aabb.hw;
        const float a_y_min = A_pos.y - A_aabb.hh;
        const float a_y_max = A_pos.y + A_aabb.hh;

        const float b_x_min = B_pos.x - B_aabb.hw; 
        const float b_x_max = B_pos.x + B_aabb.hw;
        const float b_y_min = B_pos.y - B_aabb.hh;
        const float b_y_max = B_pos.y + B_aabb.hh;

        if(b_x_max < a_x_min && b_x_min < a_x_max &&
           b_y_max < a_y_min && b_y_min < a_y_max) {
            return true;
        } 

        return false;
    }

    bool contain(const aabb_t& A_aabb, const glm::vec2& A_pos, 
                 const aabb_t& B_aabb, const glm::vec2& B_pos) {
        const float a_x_min = A_pos.x - A_aabb.hw; 
        const float a_x_max = A_pos.x + A_aabb.hw;
        const float a_y_min = A_pos.y - A_aabb.hh;
        const float a_y_max = A_pos.y + A_aabb.hh;

        const float b_x_min = B_pos.x - B_aabb.hw; 
        const float b_x_max = B_pos.x + B_aabb.hw;
        const float b_y_min = B_pos.y - B_aabb.hh;
        const float b_y_max = B_pos.y + B_aabb.hh;

        if(a_x_min <= b_x_min && b_x_max <= a_x_max &&
           a_y_min <= b_y_min && b_y_max <= a_y_max) {
            return true;
        }

        return false;
    }   
}