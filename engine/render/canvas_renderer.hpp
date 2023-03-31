#pragma once

#include "container_renderer.hpp"
#include "text_renderer.hpp"

namespace spk {
    struct canvas_renderer_t : tree_renderer_t {
        canvas_renderer_t();
        ~canvas_renderer_t();

        void pre_render() override;
        void render() override;
    
    private:
        void add_mesh(ptr_t<container_t> container);
    };
}