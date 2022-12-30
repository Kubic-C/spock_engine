#pragma once

#include "./window.hpp"
#include "systems.hpp"
#include "components/camera.hpp"
#include "render/render_system.hpp"

namespace spk {
    void render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world); 
}