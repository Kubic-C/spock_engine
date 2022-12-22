#include "sprite.hpp"
#include "../state.hpp" 

namespace spk {
    void sprite_comp_init(flecs::world& world) {
        sfk_register_component(world, comp_sprite_t);
    }
}