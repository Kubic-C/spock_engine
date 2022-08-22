#pragma once

#include "framework.hpp"
#include "utility/ui_.hpp"

namespace spk {
    class engine_t;

    using entity_reference = sfk::entity_reference;

    /* everything below should be only STRICTLY DATA, no oop */

    /* defined in their corresponding systems file */
    struct physics_scene_t;
    struct render_scene_t;
    
    /* organizes data (components) such that they are presentable to the different systems
    ** mainly the render and physics system. allowing referencing render components and physics components
    ** into a batch draw or a quad-tree, the underlying systems, will take care of the allocating, handling, deallocating
    ** of their sub scenes */
    struct scene_t {
        engine_t* engine; // the engine contaning the scene.
        sfk::window_t* window; // the output/input window that this scene is "attached" to
        flecs::world world;
        render_scene_t* render_scene;
        physics_scene_t* physics_scene;
        ui_canvas_t canvas;
    };
}