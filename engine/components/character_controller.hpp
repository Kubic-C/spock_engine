/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "components.hpp"
#include "sprite.hpp"

namespace spk {
    struct comp_character_controller_t : component_t {
        SDL_Scancode left, right, up, down;

        float speed;

        void init(flecs::entity entity);
        void free(flecs::entity entity);
    }; 

    void character_controller_comp_init(flecs::world& world);
}