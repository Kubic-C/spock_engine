#include "canvas_renderer.hpp"
#include "core/internal.hpp"

namespace spk {
    canvas_renderer_t::canvas_renderer_t() {}
    canvas_renderer_t::~canvas_renderer_t() {}

    void canvas_renderer_t::add_mesh(ptm::ptr_t<container_t> container) {
        auto& renderers          = render_context().renderers;
        auto  text_renderer      = renderers[RENDERER_TYPE_TEXT].cast<text_renderer_t>();
        auto  container_renderer = renderers[RENDERER_TYPE_CONTAINER].cast<container_renderer_t>();

        container->dimensions_calculate();
        auto& dimensions = container->dimensions_get();

        switch(container->type()) {
        case ui_types_t::CANVAS:
            break;

        case ui_types_t::BUTTON: // buttons are containers with a callback essentially
            container_renderer->button_mesh(*container.cast<button_t>());
            break;
        
        case ui_types_t::TEXT:
            text_renderer->text_mesh(*container.cast<text_t>());
            goto container_mesh;

        container_mesh:
            container_renderer->container_mesh(*container.cast<container_t>());
        };

        for(auto child : container->children) {
            add_mesh(child);
        }
    }

    void canvas_renderer_t::pre_render() {
        add_mesh(&canvas());
    }

    void canvas_renderer_t::render() {
        auto& renderers     = render_context().renderers;
        auto  text_renderer = renderers[RENDERER_TYPE_TEXT].cast<text_renderer_t>();
        auto  container_renderer = renderers[RENDERER_TYPE_CONTAINER].cast<container_renderer_t>();

        container_renderer->render();
        text_renderer->render();
    }
}