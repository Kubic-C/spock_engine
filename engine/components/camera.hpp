#pragma once

#include "framework.hpp"

namespace spk {
    struct tag_current_camera_t {};

    struct comp_camera_t {
        glm::mat4 vp;
        glm::mat4 view;
        glm::mat4 proj;

        glm::vec2 size;
        glm::vec2 pos;
        float z;

        void init();
        void free();

        void recalculate();
   }; 

   void camera_comp_init(flecs::world& world);
}