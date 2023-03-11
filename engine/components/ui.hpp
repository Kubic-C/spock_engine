#pragma once

#include "components.hpp"
#include "utility/ui.hpp"
#include "data/resource_manager.hpp"

namespace spk {
    struct engine_t;
    struct ui_button_t;
    typedef std::function<void(engine_t&, ui_button_t&)> button_callback_t;

    struct ui_tag_current_canvas_t {};

    enum ui_element_flags_e: int {
        UI_ELEMENT_FLAGS_ENABLED = (int)1 << 0,
        UI_ELEMENT_FLAGS_ROOT = (int)1 << 1,
        UI_ELEMENT_FLAGS_RELATIVE = (int)1 << 2,
        UI_ELEMENT_FLAGS_ABSOLUTE = (int)1 << 3,
        UI_ELEMENT_FLAGS_PARENT_RELATIVE = (int)1 << 4
    };

    enum ui_element_type_e: size_t {
        UI_ELEMENT_TYPE_ELEMENT = 0,
        UI_ELEMENT_TYPE_BUTTON  = 1,
        UI_ELEMENT_TYPE_TEXT    = 2,
        UI_ELEMENT_TYPE_CANVAS  = 3,
    };

    struct ui_element_t {
        glm::vec2 pos;
        glm::vec2 size;
        glm::vec3 color = { 1.0f, 0.0f, 0.0f }; // overlay color
        int flags;

        glm::vec2 abs_pos;
        glm::vec2 abs_size;

        static const uint32_t max_child_size = 5;
        std::bitset<max_child_size> in_use;
        std::array<ui_element_t*, max_child_size> elements;
        ui_element_t* parent;

        ui_element_t();
        virtual void init();
        virtual void free();

        // iterates through all children
        using children_callback_t = std::function<bool(ui_element_t&)>;
        void iter_children(children_callback_t callback);

        void gen_abs(glm::vec2 xinput_range, glm::vec2 xoutput_range,
                     glm::vec2 yinput_range, glm::vec2 youtput_range) {
            abs_pos = {
                map_value(pos.x, xinput_range, xoutput_range),
                map_value(pos.y, yinput_range, youtput_range)
            };

            abs_size = {
                map_value(size.x, xinput_range, xoutput_range),
                map_value(size.y, yinput_range, youtput_range)
            };
        }

        uint32_t add_child(ui_element_t* child) {
            spk_assert(child);
            
            if(in_use.count() >= max_child_size)
                return UINT32_MAX;
            
            for(uint32_t i = 0; i < max_child_size; i++ ) {
                if(!in_use.test(i)) {
                    return set_child(i, child);
                } 
            }

            return UINT32_MAX;
        }

        uint32_t set_child(uint32_t index, ui_element_t* child) {
            spk_assert(index < max_child_size && child != null && !in_use.test(index)); 

            in_use.set(index, true);

            elements[index] = child;
            child->parent = this;

            return index;
        }

        ui_element_t* remove_child(uint32_t index) {
            spk_assert(index < max_child_size && in_use.test(index)); 
            
            ui_element_t* element;

            in_use.set(index, false);

            element = elements[index];
            elements[index] = nullptr;

            return element;
        }

        virtual const size_t type() { return UI_ELEMENT_TYPE_ELEMENT; }
    };

    struct ui_text_t : ui_element_t {
        text_t text;

        const size_t type() override { return UI_ELEMENT_TYPE_TEXT; }
    };

    struct ui_button_t : ui_element_t {
        button_callback_t callback = nullptr;
        float time_when_clicked = 0.0f;

        const size_t type() override { return UI_ELEMENT_TYPE_BUTTON; }
    };

    struct ui_comp_canvas_t : ui_element_t {
        glm::mat4 vp;

        font_t* font;
        std::list<ui_text_t, memory_pool_t<ui_text_t>> texts;
        std::list<ui_button_t, memory_pool_t<ui_button_t>> btns;

        ui_comp_canvas_t() {
            font = null;
        }

        void init(flecs::entity entity);
        void free(flecs::entity entity);

        void resize_callback(int width, int height);
        const size_t type() override { return UI_ELEMENT_TYPE_CANVAS; }
    };

    void ui_canvas_comp_init(flecs::world& world);
}