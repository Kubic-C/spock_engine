#include "ui_.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    glm::vec2 determine_position(glm::vec2 size, glm::vec2 pos) {
        pos.x = map_value(pos.x, {-1.0f, 1.0f}, {0, size.x});
        pos.y = map_value(pos.y, {-1.0f, 1.0f}, {0, size.y});

        return pos; 
    }

    glm::vec2 keep_inside_box(glm::vec2 size, glm::vec2 box_size, glm::vec2 pos) {
        if(pos.x + box_size.x > size.x) {
            pos.x = size.x - box_size.x;
        } 
        
        if(pos.y + box_size.y > size.y) {
            pos.y = size.y - box_size.y;
        }       

        return pos;
    }
}