#include "ui.hpp"
#include "internal.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    void canvas_make_current(canvas_t& canvas) {
        internal->scene.canvas = &canvas;        
    }
}