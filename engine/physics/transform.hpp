#pragma once

#include "base.hpp"
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    struct transform_t {
        glm::mat4 mat;
        glm::vec2 pos;
        float     rot; // in radians

        transform_t() {
            pos = { 0.0f, 0.0f };
            rot = 0.0f;
            calculate();
        }

        void calculate() {
            mat = glm::identity<glm::mat4>();
            mat = glm::translate(mat, glm::vec3(pos, 0.0f));
            mat = glm::rotate(mat, rot, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        glm::vec2 rotate(const glm::vec2& v2) {
            const glm::vec3 v3 = 
                glm::rotate(glm::identity<glm::mat4>(), rot, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(v2, 0.0f, 1.0f); 

            return glm::vec2(v3.x, v3.y);
        }

        glm::vec2 operator*(const glm::vec2& v2) {
            const glm::vec3 v3 = mat * glm::vec4(v2, 0.0f, 1.0f); 

            return glm::vec2(v3.x, v3.y);
        }
    };
}