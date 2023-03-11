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