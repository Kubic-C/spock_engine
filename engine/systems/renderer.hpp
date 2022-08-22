#pragma once

#include "primitive_renderer.hpp"
#include "ui_renderer.hpp"

namespace spk {
    struct render_scene_t {
        quad_scene_t quad_batch;
    };

    class renderer2D_t : public system_t {
    public:
        renderer2D_t();

        void init(scene_t& scene, void* data);
        void update(scene_t& scene, float deltatime);
        void tick(scene_t& scene, float deltatime) {}
        static void resize(sfk::window_t* window, void* self, int width, int height);
        void free(scene_t& scene);

        render_system_manager_t renderer_manager;

    private:
        // need a place to access each render system's
        // non-base-class methods
        primitive_renderer_t primitive_renderer;
        ui_renderer_t ui_renderer;
    };
}