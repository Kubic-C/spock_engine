#pragma once

#include "text.hpp"
#include "utility/ui.hpp"
#include "utility/stack_allocator.hpp"

namespace spk {
    struct canvas_t {

    };

    /**
     * @brief sets the passed canvas as the current canvas
     * 
     * @param canvas the UI canvas that you want to make active
     */
    void canvas_make_current(canvas_t& canvas);
}