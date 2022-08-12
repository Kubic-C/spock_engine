#include "ui_.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    void ui_axises_tt::set(glm::vec2 position, glm::vec2 size) {
        this->position = position;
        this->size = size;

        half_width = this->size.x / 2.0f;
        half_height = this->size.y / 2.0f;
    }

    void ui_canvas_tt::init() {
        DEBUG_VALUE(bool, ret =) texts.init(sfk::xor_int_hash<sfk::key_tt>);
        assert(ret);
        DEBUG_EXPR(ret =) buttons.init(sfk::xor_int_hash<sfk::key_tt>);
        assert(ret);

        font = UINT32_MAX;
        // vp = glm::identity<glm::mat4>(); this will be set in a resize
    }

    void ui_canvas_tt::resize(int width, int height) {
        glm::mat4 view, proj;

        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, 0.01f, 100.0f);

        vp = proj * view;
    }

    void ui_canvas_tt::free() {
        texts.free();
    }

    ui_button_tt* ui_canvas_tt::init_button(sfk::key_tt key, ui_axises_tt axises, glm::vec3 color, 
            button_callback_tt clbk, ui_axises_tt* parent) {
        ui_button_tt* btn;

        if(!buttons.register_key(key))
            return nullptr;

        btn = &buttons[key];
        btn->color = color;
        btn->axises = axises;
        btn->parent = parent;
        btn->time_when_clicked = 0.0f;
        btn->callback = clbk;

        return btn;
    }

    ui_text_tt* ui_canvas_tt::init_text(sfk::key_tt key, ui_axises_tt axises, glm::vec3 color, const char* str, ui_axises_tt* parent) {
        ui_text_tt* txt;

        if(!texts.register_key(key))
            return nullptr;

        txt = &texts[key];
        txt->color = color;
        txt->axises = axises;
        txt->parent = parent;
        txt->color = color;
        txt->str = str;

        return txt;
    }
}