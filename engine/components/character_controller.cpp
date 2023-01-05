#include "character_controller.hpp"

namespace spk {
    void comp_character_controller_t::init() {
        speed       = 1.0f;
    
        left  = SDL_SCANCODE_A;
        right = SDL_SCANCODE_D;
        up    = SDL_SCANCODE_W;
        down  = SDL_SCANCODE_S;
    }
    
    void comp_character_controller_t::free() {
    }

    void character_controller_comp_init(flecs::world& world) {
        spk_register_component(world, comp_character_controller_t);
    }
}