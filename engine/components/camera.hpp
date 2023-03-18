#pragma once

#include "components.hpp"

namespace spk {
    struct tag_current_camera_t {};

    struct comp_camera_t : component_t { 
        float scale;
        glm::vec2 size;
        glm::vec2 pos;
        float z;

        void init(flecs::entity entity);
        void free(flecs::entity entity);
        glm::vec2 get_world_position(const glm::vec2& screen_coords);

        void recalculate();
   }; 

   void camera_comp_init(flecs::world& world);
}