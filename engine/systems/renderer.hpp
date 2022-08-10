#pragma once

#include "primitive_renderer.hpp"
#include "ui_renderer.hpp"

namespace spk {
    struct render_scene_tt {
        quad_scene_tt quad_batch;
    };

    class renderer2D_tt : public system_tt {
    public:
        renderer2D_tt();

        void init(scene_tt& scene, void* data);
        void update(scene_tt& scene, float deltatime);
        void tick(scene_tt& scene, float deltatime) {}
        static void resize(sfk::window_tt* window, void* self, int width, int height);
        void free(scene_tt& scene);

        render_system_manager_tt renderer_manager;

    private:
        // need a place to access each render system's
        // non-base-class methods
        primitive_renderer_tt primitive_renderer;
        ui_renderer_tt ui_renderer;
    };
}