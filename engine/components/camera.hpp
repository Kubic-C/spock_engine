/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

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

   void camera_comp_init(const flecs::world& world);
}