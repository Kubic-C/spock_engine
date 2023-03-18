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
}