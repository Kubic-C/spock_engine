#include "ui.hpp"
#include "state.hpp"
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

    void ui_comp_canvas_t::init(flecs::entity entity) {
        font = nullptr;

        flags |= spk::UI_ELEMENT_FLAGS_ROOT |
                 spk::UI_ELEMENT_FLAGS_ENABLED;

        size = { std::nanf("nan"), std::nanf("nan") };
        pos = { std::nanf("nan"), std::nanf("nan") };
    }

    void ui_comp_canvas_t::free(flecs::entity entity) {

    }

    void ui_comp_canvas_t::resize_callback(int width, int height) {
        glm::mat4 view, proj;

        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, 0.01f, 100.0f);

        vp = proj * view;
 
        abs_size = { (float)width, (float)height };
    }

    void ui_tag_current_canvas_on_add(flecs::entity e, ui_comp_canvas_t& canvas) {
        if(state.get_current_canvas().id() != UINT64_MAX) {
            state.get_current_canvas().remove<ui_tag_current_canvas_t>();
        }

        state.set_current_canvas(e);
    }

    void ui_canvas_comp_init(flecs::world& world) {
        spk_register_component(world, ui_comp_canvas_t);

        world.observer<ui_comp_canvas_t>().term<ui_tag_current_canvas_t>()
            .event(flecs::OnAdd).each(ui_tag_current_canvas_on_add);
    }
}