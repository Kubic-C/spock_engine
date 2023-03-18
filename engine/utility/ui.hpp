/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

    refer to license: https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "base.hpp"

namespace spk {
    inline float map_value(float value, glm::vec2 start, glm::vec2 end) {
        const float start1 = start.x;
        const float start2 = start.y;
        const float stop1   = end.x;
        const float stop2   = end.y;

        float ret = stop1 + (value - start1) * (stop2 - stop1) / (start2 - start1);

        return ret;
    }

    // this will give you the absolute coords of a box inside of size.
    glm::vec2 determine_position(glm::vec2 size, glm::vec2 pos);

    glm::vec2 keep_inside_box(glm::vec2 size, glm::vec2 box_size, glm::vec2 pos);

    inline bool vec2_aabb(const glm::vec2 b1[2], const glm::vec2 b2[2]) {
        return (b1[0][0] <= b2[1][0] && b1[1][0] >= b2[0][0]) 
            && (b1[0][1] <= b2[1][1] && b1[1][1] >= b2[0][1]);
    }
}