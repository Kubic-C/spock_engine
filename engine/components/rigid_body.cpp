#include "rigid_body.hpp"

namespace spk { 
    void rigid_body_comp_init(flecs::world& world) {
        spk_register_component(world, comp_rigid_body_t);
    }
}