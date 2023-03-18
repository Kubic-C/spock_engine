#pragma once

#include "render.hpp"
#include "components/sprite.hpp"
#include "systems/particles.hpp"
#include "systems/tilemap.hpp"

namespace spk {
    void sprite_render_init(flecs::world& world);
}