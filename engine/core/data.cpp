/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "data.hpp"
#include "core/internal.hpp"

namespace spk {
    flecs::world& ecs_world() {
        return internal->scene.ecs_world;
    }

    b2World* physics_world() {
        return internal->scene.physics_world;
    }

    window_t& window() {
        return *internal->scene.window;
    }

    canvas_t& canvas() {
        return *internal->scene.canvas;
    }

    settings_t& settings() {
        return internal->settings;
    }

    statistics_t& statistics() {
        return internal->statistics;
    }

    scene_t& scene() {
        return internal->scene;
    }

    resources_t& resources() {
        return internal->resources;
    }

    allocators_t& allocators() {
        return internal->allocators;
    }
}