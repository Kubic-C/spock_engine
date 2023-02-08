#include "body.hpp"

namespace spk {
    aabb_pos_t rigid_body_t::get_aabb_pos() const {
        return aabb_pos_t(transform.pos + max.pos, max.aabb);
    }

    void rigid_body_t::calculate_max() {
        float right_most  = -FLOAT_MAX;
        float left_most   =  FLOAT_MAX;
        float top_most    = -FLOAT_MAX;
        float bottom_most =  FLOAT_MAX;
        std::vector<glm::vec2> vertices;
        
        glm::vec2 median = {0.0f, 0.0f};

        if(fixtures_size == 0) {
            max.aabb.hh = 0.0f;
            max.aabb.hw = 0.0f;
            max.pos     = {0.0f, 0.0f};
            return;
        }

        vertices.reserve(5);

        fixture_t* cur = fixtures;
        for(size_t i = 0; i < fixtures_size; i++) {
            switch(cur->type) {
            case FIXTURE_TYPE_BOX: {
                vertices.push_back(cur->def.box.bl() + cur->pos);
                vertices.push_back(cur->def.box.br() + cur->pos);
                vertices.push_back(cur->def.box.tr() + cur->pos);
                vertices.push_back(cur->def.box.tl() + cur->pos);
            } break;

            default:
                break;
            }

            if(cur->next != nullptr) {
                cur = cur->next;
            } else {
                break;
            }
        }

        for(const auto& vertex : vertices) {
            if(vertex.x < left_most)
                left_most = vertex.x;
            else if(vertex.x > right_most)
                right_most = vertex.x;

            if(vertex.y < bottom_most)
                bottom_most = vertex.y;
            else if(vertex.y > top_most)
                top_most = vertex.y;
        
            median.x += vertex.x;
            median.y += vertex.y;
        }

        max.aabb.hh = (top_most - bottom_most) / 2.0f;
        max.aabb.hw = (right_most - left_most) / 2.0f;
        max.pos     = median / (float)vertices.size();
    }
}