#pragma once

#include "components/camera.hpp"
#include "systems.hpp"

namespace spk {
    struct camera_system_ctx_t {
        SPK_NOT_A_TAG
    };

    void camera_cs_init(system_ctx_allocater_t& allocater, flecs::world& world);    
}