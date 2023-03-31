#include "canvas_renderer.hpp"
#include "core/internal.hpp"

namespace spk {
    canvas_renderer_t::canvas_renderer_t() {}
    canvas_renderer_t::~canvas_renderer_t() {}

    void canvas_renderer_t::add_mesh(ptr_t<container_t> container) {
        auto& renderers          = render_context().renderers;
        auto  text_renderer      = renderers[RENDERER_TYPE_TEXT].cast<text_renderer_t>();
        auto  container_renderer = renderers[RENDERER_TYPE_CONTAINER].cast<container_renderer_t>();

        glm::vec3 pos  = container->pos;
        glm::vec2 size = container->size;

        switch(container->type()) {
        case ui_types_t::CANVAS:
            break;

        case ui_types_t::BUTTON:
            goto container_mesh;
        
        case ui_types_t::TEXT:
            text_renderer->text_mesh(pos, size, *container.cast<text_t>());
            goto container_mesh;

        container_mesh:
            container_renderer->container_mesh(pos, size, *container.cast<container_t>());
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