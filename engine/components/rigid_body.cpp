/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "rigid_body.hpp"
#include "core/internal.hpp"

namespace spk { 
    void comp_rigid_body_t::init(flecs::entity entity) {
        this->body = physics_world().create_rigid_body({0.0f, 0.0f}, 0.0f, kin::body_type_dynamic);
    }

    void comp_rigid_body_t::free(flecs::entity entity) {
        if(body) {
            physics_world().destroy_rigid_body(body);
        }
    }

    void rigid_body_comp_init(const flecs::world& world) {
        spk_register_component(world, comp_rigid_body_t);
    }
}