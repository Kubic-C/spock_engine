#pragma once

#include "sprite.hpp"

namespace spk {
    struct comp_character_controller_t {
        comp_sprite_atlasd_t sprite;

        SDL_Scancode left, right, up, down;

        float density;
        float friction;
        float restitution;
        float speed;

        void init();
        void free();
        void add_fixture(b2Body* body);
    }; 

    void character_controller_comp_init(flecs::world& world);
}