#include "base_renderer.hpp"

namespace spk {
    void render_ctx_t::init() {
        vertex_array.init();
        program.init();
    }

    void render_ctx_t::free() {
        program.free();
        vertex_array.free();
    }
}