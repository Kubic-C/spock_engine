#pragma once

#include "base.hpp"

namespace spk {
    struct comp_rigid_body_t {
        b2Body* body;

        void init(flecs::entity entity) {}
        void free(flecs::entity entity) {
            if(body) {
                body->GetWorld()->DestroyBody(body);
            }
        }

        b2Body* operator->() {
            return body;
        }

        b2Body& operator*() {
            return *body;
        }

        operator b2Body*() {
            return body;
        }

        void operator=(b2Body* other) {
            body = other;
        }
    };

    void rigid_body_comp_init(flecs::world& world);
}