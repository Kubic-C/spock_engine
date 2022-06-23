#pragma once

#include "base.hpp"

namespace sfk {
    uint16_t get_generation_count(uint64_t id);

    struct entity_reference {
        flecs::entity_t e;

        bool is_alive(flecs::world& world) {
            flecs::entity entity(e);
            ecs_id_t id_new = world.get_alive(entity);

            if(get_generation_count(entity.id()) != get_generation_count(id_new))
                return false;
            else
                return true;
        }

        flecs::entity operator->() {
            return flecs::entity(e);
        }
    };
}