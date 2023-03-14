#include "ui.hpp"
#include "internal.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    ui_element_t::ui_element_t() {
        spk::zero(&pos);
        spk::zero(&size);
        spk::zero(&flags);

        in_use.reset();
        elements.fill(nullptr);
        parent = nullptr;   
        abs_pos  = { 0.0f, 0.0f };
    }

    void ui_element_t::init() {
    }

    void ui_element_t::iter_children(children_callback_t callback) {
        for(auto ele : elements) {
            if(ele) {
                if(callback(*ele)) {
                    continue;
                } else {
                   ele->iter_children(callback);
                }
            }
        }
    }

    void ui_element_t::free() {

    }

    ui_canvas_t::ui_canvas_t() {
        font = nullptr;

        flags |= spk::UI_ELEMENT_FLAGS_ROOT |
                 spk::UI_ELEMENT_FLAGS_ENABLED;

        size = { std::nanf("nan"), std::nanf("nan") };
        pos = { std::nanf("nan"), std::nanf("nan") };
    }

    ui_canvas_t::~ui_canvas_t() {

    }

    void ui_canvas_t::resize_callback(int width, int height) {
        glm::mat4 view, proj;

        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, 0.01f, 100.0f);

        vp = proj * view;
 
        abs_size = { (float)width, (float)height };
    }

    ui_canvas_t& canvas() {
        return *internal->scene.canvas;
    }

    void canvas_make_current(ui_canvas_t& canvas) {
        if(internal->scene.canvas != nullptr) {
            // do something, here for future use
        }

        internal->scene.canvas = &canvas;
    }
}