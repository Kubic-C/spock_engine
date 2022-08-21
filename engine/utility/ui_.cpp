#include "ui_.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    void ui_element_tt::_init_members() {
        sfk::zero(&pos);
        sfk::zero(&size);
        sfk::zero(&flags);

        in_use.reset();
        elements.fill(nullptr);
        parent = nullptr;   
        abs_pos  = { 0.0f, 0.0f };
    }

    void ui_element_tt::init() {
        _init_members();
    }

    bool ui_element_tt::iter_children(children_callback_tt callback) {
        for(auto ele : elements) {
            if(ele) {
                if(callback(*ele)) {
                    return true;
                } 

                if(ele->iter_children(callback)) {
                    return true;
                }
            }
        }

        return false;
    }

    void ui_element_tt::free() {

    }

    void ui_canvas_tt::init() {
        _init_members();

        DEBUG_VALUE(bool, ret =) texts.init();
        assert(ret);
        DEBUG_EXPR(ret =) btns.init();
        assert(ret);

        font = nullptr;

        flags |= spk::UI_ELEMENT_FLAGS_ROOT |
                 spk::UI_ELEMENT_FLAGS_ENABLED;

        size = { std::nanf("nan"), std::nanf("nan") };
        pos = { std::nanf("nan"), std::nanf("nan") };
    }

    void ui_canvas_tt::resize(int width, int height) {
        glm::mat4 view, proj;

        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, 0.01f, 100.0f);

        vp = proj * view;
 
        abs_size = { (float)width, (float)height };
    }

    void ui_canvas_tt::free() {
        texts.free();
        btns.free();
    }
}