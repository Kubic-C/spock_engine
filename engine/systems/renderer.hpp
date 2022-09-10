#pragma once

#include "collider_renderer.hpp"
#include "ui_renderer.hpp"
#include "tile_renderer.hpp"

namespace spk {
    struct render_scene_t {
        tile_render_scene_t tile_scene;

        glm::mat4 vp;
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
        scene_t* scene;

    private:
        // need a place to access each render system's
        // non-base-class methods
        collider_renderer_t primitive_renderer;
        ui_renderer_t ui_renderer;
        tile_renderer_t tile_renderer;
    };
}