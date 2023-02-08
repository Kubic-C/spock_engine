#pragma once

#include "transform.hpp"

namespace spk {
    struct aabb_t {
        float hw;
        float hh;

        aabb_t();
        aabb_t(float x, float y);
        aabb_t(const glm::vec2& other);
        
        glm::vec2 vec2() const {
            return glm::vec2(hw, hh);
        }

        glm::vec2 bl() {
            return {-hw, -hh};
        }

        glm::vec2 br() {
            return { hw, -hh};
        }

        glm::vec2 tr() {
            return { hw, hh};
        }

        glm::vec2 tl() {
            return {-hw, hh};
        }
    };

    struct aabb_pos_t {
        aabb_t    aabb;
        glm::vec2 pos;

        aabb_pos_t() {
            pos = {0.0f, 0.0f};
        }

        aabb_pos_t(const glm::vec2& pos, const aabb_t& aabb) {
            this->pos  = pos;
            this->aabb = aabb;
        }
    };

    bool overlap(const aabb_t& A_aabb, const glm::vec2& A_pos, 
                 const aabb_t& B_aabb, const glm::vec2& B_pos);

    // can A contain B?
    bool contain(const aabb_t& A_aabb, const glm::vec2& A_pos, 
                 const aabb_t& B_aabb, const glm::vec2& B_pos);

    inline bool overlap(const aabb_pos_t& A_aabb, const aabb_pos_t& B_aabb) {
        return overlap(A_aabb.aabb, A_aabb.pos, B_aabb.aabb, B_aabb.pos);
    }
    
    // can A contain B?
    inline bool contain(const aabb_pos_t& A_aabb, const aabb_pos_t& B_aabb) {
        return contain(A_aabb.aabb, A_aabb.pos, B_aabb.aabb, B_aabb.pos);
    }}