#pragma once

#include "framework.hpp"

namespace spk {
    typedef void(*character_callback_t)(flecs::entity entity, uint32_t character);

    /* function pointers to callbacks- all user set
    ** probably the basis of implementing some sort of scripting system */
    struct callback_t {
        character_callback_t fp_char_callback;
    };
}
