#pragma once

#include "../utility/ui_.hpp"

// uic == UI component

namespace spk {
    struct ui_tag_active_element_t {};
    struct ui_tag_current_canvas_t {};

    struct ui_comp_attribute_position_t : ui_attribute_base_t {
        bool absolute;
        glm::vec2 position;

        ui_comp_attribute_position_t() {
            precedence = 1;
            absolute = false;
            position = {0.0f, 0.0f};
        }
    };  

    struct ui_comp_attribute_text_t : ui_attribute_base_t {
        glm::vec3 color;
        std::string str;
    
        ui_comp_attribute_text_t() {
            precedence = 0;
            color = {1.0f, 1.0f, 1.0f};
            str = "";
        }
    };

    struct ui_comp_attribute_size_t : ui_attribute_base_t {
        bool absolute;
        glm::vec2 size;

        ui_comp_attribute_size_t() {
            precedence = 0;
            size = {0.0f, 0.0f};
            absolute = false;
        }
    };

    struct ui_comp_t {
        glm::vec3 color = { 1.0f, 0.0f, 0.0f };
        bool centered = true;

        glm::vec2 padding; // x- left and right, y- top and bottom
        glm::vec2 margin; // x- left and right, y- top and bottom

        struct cache_t {    
            bool update = false;
            glm::vec2 position; // absolute position
            glm::vec2 size; // absolute size
        } _cache;
    };

    /** structure of a entity thats a canvas' elements
     * ui_comp_t
     * ui_attribute_t (type text)
     * ui_attribute_t (type position)
     */
    struct ui_comp_canvas_t {
        flecs::entity font;
        bool update; // update the canvas; re render framebuffer

        // TODO framebuffer saves
        uint32_t framebuffer;
    };

    void ui_comp_canvas_init(flecs::world& world);
}