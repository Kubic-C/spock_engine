#pragma once

#include "resource_manager.hpp"

namespace spk {
    typedef void(*button_callback_tt)();

    struct ui_button_tt;

    struct ui_text_tt {
        ui_button_tt* button;

        glm::vec2 position = { 0.0f, 0.0f };
        glm::vec3 color = { 1.0f, 0.0f, 0.0f };
        std::string str = "";
    };

    struct ui_button_tt {
        ui_text_tt* text;

        glm::vec2 position;
        glm::vec2 size;
        glm::vec3 color;
        button_callback_tt callback;
        float time_when_clicked = 0.0f;
    };

    struct ui_canvas_tt { 
        void init();
        void free();
        void resize(int width, int height);

        glm::mat4 vp;

        uint32_t font;

        sfk::hashmap_tt<ui_text_tt, 24, 2, 0> texts;
        sfk::hashmap_tt<ui_button_tt, 12, 2, 0> buttons;
    };

}