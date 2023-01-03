#include "character_controller.hpp"

namespace spk {
    void comp_character_controller_t::init() {
        restitution = 0.1f;
        density     = 1.0f;
        friction    = 0.2f;
        speed       = 1.0f;
    
        left  = SDL_SCANCODE_A;
        right = SDL_SCANCODE_D;
        up    = SDL_SCANCODE_W;
        down  = SDL_SCANCODE_S;

        sprite.init();
    }
    
    void comp_character_controller_t::free() {
        sprite.free();
    }

    void comp_character_controller_t::add_fixture(b2Body* body) {
        sprite.add_fixture(body, density, friction, restitution);
    }

    void character_controller_comp_init(flecs::world& world) {
        spk_register_component(world, comp_character_controller_t);
    }
}