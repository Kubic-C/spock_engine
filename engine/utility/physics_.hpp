#pragma once

#include "framework.hpp"

namespace spk {
    constexpr auto NULL_HANDLE = nullptr;

    // flecs is dumb when it comes to component types as pointers, so we gotta do some shit like this ;p
    struct comp_b2Body { b2Body* body = nullptr; }; 
    
    /* add OnAdd and OnDestroy callbacks to a flecs world*/
    void add_box2d_components(flecs::world& ecs_world, b2World* phy_world);
}