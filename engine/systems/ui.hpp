#pragma once

#include "../systems.hpp"

namespace spk {
    /* user interface oooohhhh look at mee ooooh*/
    class ui_t : public system_t {
    public:
        void init(scene_t& scene, void* data);
        void update(scene_t& scene, float deltatime);
        void tick(scene_t& scene, float deltatime);
        void free(scene_t& scene);

        static void mouse_button_callback(sfk::window_t* window, void* self, int button, int action, int mods);

        scene_t* scene;
    private:
    };
}