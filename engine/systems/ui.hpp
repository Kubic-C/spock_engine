#pragma once

#include "core/ui.hpp"

#include "render/font_renderer.hpp"
#include "render/button_renderer.hpp"
#include "render/ui_fb_renderer.hpp"
#include "render/render_system.hpp"

#include "utility/stack_allocator.hpp"

namespace spk {
    struct ui_meshes_t {
        font_batch_mesh_t*   font_mesh;
        button_batch_mesh_t* btn_mesh;
    };

    void ui_cs_init(flecs::world& world);
}