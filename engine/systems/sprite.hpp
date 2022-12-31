#pragma once

#include "render.hpp"
#include "components/sprite.hpp"
#include "render/sprite_renderer.hpp"
#include "systems/particles.hpp"
#include "systems/tilebody.hpp"

namespace spk {
    void sprite_cs_init(system_ctx_allocater_t& allocater, flecs::world& world);
}