#pragma once

#include "sprite.hpp"

namespace spk {
    struct comp_character_controller_t {
        SDL_Scancode left, right, up, down;

        float speed;

        void init();
        void free();
    }; 

    void character_controller_comp_init(flecs::world& world);
}