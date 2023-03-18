/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

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