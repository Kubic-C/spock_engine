/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "ui.hpp"
#include "internal.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    void canvas_make_current(canvas_t& canvas) {
        internal->scene.canvas = &canvas;        
    }
}