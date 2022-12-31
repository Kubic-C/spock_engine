#pragma once

#include "render/primitive_renderer.hpp"

namespace spk {
    void primitive_render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world);
}