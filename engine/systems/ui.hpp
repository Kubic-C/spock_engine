#pragma once

#include "systems.hpp"
#include "components/ui.hpp"

#include "render/font_renderer.hpp"
#include "render/button_renderer.hpp"
#include "render/ui_fb_renderer.hpp"
#include "render/render_system.hpp"

namespace spk {
    struct ui_meshes_t : public system_t {
        font_batch_mesh_t*   font_mesh;
        button_batch_mesh_t* btn_mesh;
    };

    struct ui_system_ctx_t : public system_t {
        SPK_NOT_A_TAG; 
    };

    void ui_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world);
}