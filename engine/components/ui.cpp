#include "ui.hpp"
#include "../state.hpp"

namespace spk {
    void ui_comp_canvas_on_add(ui_comp_canvas_t& canvas) {
        
    }

    void ui_comp_canvas_on_remove(ui_comp_canvas_t& canvas) {

    }

    void ui_tag_current_canvas_on_add(flecs::entity e, ui_comp_canvas_t& canvas) {
        if(state._get_current_canvas().id() != UINT64_MAX) {
            state._get_current_canvas().remove<ui_tag_current_canvas_t>();
        }

        state._set_current_canvas(e);
    }


    void ui_comp_canvas_init(flecs::world& world) {
        world.component<ui_comp_t>();
        world.component<ui_comp_canvas_t>();
        world.observer<ui_comp_canvas_t>().event(flecs::OnAdd).each(ui_comp_canvas_on_add);
        world.observer<ui_comp_canvas_t>().event(flecs::OnRemove).each(ui_comp_canvas_on_remove);
        world.observer<ui_comp_canvas_t>().term<ui_tag_current_canvas_t>()
            .event(flecs::OnAdd).each(ui_tag_current_canvas_on_add);
    }
}