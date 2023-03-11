#include "rigid_body.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk { 
    void comp_rigid_body_t::init(flecs::entity entity) {
        b2BodyDef def;
        
        body = state.get_current_physics_world()->CreateBody(&def);

        auto& ud = body->GetUserData();
        ud.entity = entity;
        ud.pointer = 0;
    }

    void rigid_body_comp_init(flecs::world& world) {
        spk_register_component(world, comp_rigid_body_t);
    }
}