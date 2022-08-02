#pragma once

#include "framework.hpp"

namespace spk {
    typedef void(*character_callback_tt)(flecs::entity entity, uint32_t character);

    /* function pointers to callbacks- all user set
    ** probably the basis of implementing some sort of scripting system */
    struct callback_tt {
        character_callback_tt fp_char_callback;
    };
}
