#pragma once

#include "../systems.hpp"

namespace spk {
    /* user interface oooohhhh look at mee ooooh*/
    class ui_tt : public system_tt {
    public:
        void init(scene_tt& scene, void* data);
        void update(scene_tt& scene, float deltatime);
        void tick(scene_tt& scene, float deltatime);
        void free(scene_tt& scene);

        static void mouse_button_callback(sfk::window_tt* window, void* self, int button, int action, int mods);

        ui_canvas_tt* canvas;
    private:
        uint32_t text_key = 0;
        uint32_t button_key = 0;
    };
}