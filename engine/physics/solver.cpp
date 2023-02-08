#include "solver.hpp"

namespace spk {
    bool detect_collision(rigid_body_t* A, rigid_body_t* B) {
        if(overlap(A->max.aabb, A->transform.pos + A->max.pos, B->max.aabb, B->transform.pos + B->max.pos)) {
            return true;
        } else {
            return false;
        }
    }
}