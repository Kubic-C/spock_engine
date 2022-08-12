#pragma once

#include "resource_manager.hpp"

namespace spk {
    struct ui_button_tt;
    struct scene_tt;
    typedef void(*button_callback_tt)(scene_tt& scene, ui_button_tt* btn);

    /* all origons start in the bottom left corner*/

    enum ui_relative_positions_e {
        BOTTOM_LEFT = 0,
        BOTTOM_RIGHT = 1,
        TOP_LEFT = 2,
        TOP_RIGHT = 3,
    };

    struct ui_axises_tt {
        glm::vec2 position;
        glm::vec2 size = { 1.0f, 1.0f }; // size in text is a scalar

        float half_width;
        float half_height;

        void set(glm::vec2 position, glm::vec2 size = {0.0f, 0.0f});
    };

    struct ui_text_tt {
        ui_axises_tt axises;
        ui_axises_tt* parent;
        
        glm::vec3 color = { 1.0f, 0.0f, 0.0f };
        std::string str = "";

        struct {
            float width;
            float height;
        } _render;
    };

    struct ui_button_tt {
        ui_axises_tt axises;
        ui_axises_tt* parent;
        button_callback_tt callback;

        glm::vec3 color = { 1.0f, 1.0f, 1.0f };
        float time_when_clicked = 0.0f;
    };

    struct ui_canvas_tt { 
        void init();
        void free();
        void resize(int width, int height);

        ui_button_tt* init_button(sfk::key_tt key, ui_axises_tt axises, glm::vec3 color, 
            button_callback_tt clbk, ui_axises_tt* parent = nullptr);
        ui_text_tt* init_text(sfk::key_tt key, ui_axises_tt axises, glm::vec3 color, const char* str, 
            ui_axises_tt* parent = nullptr);

        glm::mat4 vp;

        uint32_t font;

        sfk::hashmap_tt<ui_text_tt, 24, 2, 0> texts;
        sfk::hashmap_tt<ui_button_tt, 12, 2, 0> buttons;
    };
}