#include "sprite.hpp"
#include "../state.hpp" 

namespace spk {
    void sprite_comp_init(flecs::world& world) {
        spk_register_component(world, comp_sprite_t);
    }
}