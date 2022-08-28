#pragma once

#include "framework.hpp"
#include "utility/ui_.hpp"

namespace spk {
    class engine_t;

    using entity_reference = sfk::entity_reference;

    typedef uint32_t user_game_state_t;

    /* everything below should be only STRICTLY DATA, no oop */

    /* defined in their corresponding systems file */
    struct physics_scene_t;
    struct render_scene_t;
    
    /* a data structure given to all systems, it simply contains data that controls
       what the game "looks" like, or how it "behaves" */
    struct scene_t {
        engine_t* engine; // the engine contaning the scene.
        sfk::window_t* window; // the output/input window that this scene is "attached" to
        flecs::world world;
        render_scene_t* render_scene;
        physics_scene_t* physics_scene;
        ui_canvas_t canvas;
        user_game_state_t user_game_state;
    };
}