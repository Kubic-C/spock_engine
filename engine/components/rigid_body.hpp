/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "components.hpp"

namespace spk {
    struct comp_rigid_body_t : component_t {
        kin::rigid_body_t* body = nullptr;

        void init(flecs::entity entity);

        void free(flecs::entity entity);

        kin::rigid_body_t* operator->() {
            return body;
        }

        kin::rigid_body_t& operator*() {
            return *body;
        }

        operator kin::rigid_body_t*() {
            return body;
        }

        void operator=(kin::rigid_body_t* other) {
            body = other;
        }
    };

    void rigid_body_comp_init(const flecs::world& world);
}