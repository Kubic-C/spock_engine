#pragma once

#include "./window.hpp"
#include "components/camera.hpp"
#include "render/render_system.hpp"

namespace spk {
    void render_cs_init(flecs::world& world); 
}