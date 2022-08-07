#pragma once

#include "framework.hpp"

namespace spk {
    class engine_tt;

    using entity_reference = sfk::entity_reference;

    /* everything below should be only STRICTLY DATA, no oop */

    /* defined in their corresponding systems file */
    struct physics_scene_tt;
    struct render_scene_tt;
    
    /* organizes data (components) such that they are presentable to the different systems
    ** mainly the render and physics system. allowing referencing render components and physics components
    ** into a batch draw or a quad-tree, the underlying systems, will take care of the allocating, handling, deallocating
    ** of their sub scenes */
    struct scene_tt {
        engine_tt* engine; // the engine contaning the scene.
        sfk::window_tt* window; // the output/input window that this scene is "attached" to
        flecs::world world;
        render_scene_tt* render_scene;
        physics_scene_tt* physics_scene;
    };
}