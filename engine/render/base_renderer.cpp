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


   void base_renderer_t::b_init() {
   } 

   void base_renderer_t::b_free() {
   }
}