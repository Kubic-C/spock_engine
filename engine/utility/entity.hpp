#pragma once

#include "framework.hpp"

namespace spk {
    template<typename T, typename func>
    void add_component(flecs::world& world, flecs::entity e, const func& set) {
        e.add<T>(); // FLECS does not call the on_add hook of a component before calling set
        world.defer([&](){e.set<func>(set);});
    }
}