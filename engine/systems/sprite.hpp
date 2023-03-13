#pragma once

#include "render.hpp"
#include "components/sprite.hpp"
#include "render/sprite_renderer.hpp"
#include "systems/particles.hpp"
#include "systems/tilemap.hpp"
#include "systems/character_controller.hpp"

namespace spk {
    void sprite_cs_init(flecs::world& world);
}