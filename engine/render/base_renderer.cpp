#include "base_renderer.hpp"

namespace spk {
   void base_renderer_t::b_init() {
        vertex_array.init();
        program.init();
   } 

   void base_renderer_t::b_free() {
        program.free();
        vertex_array.free();
   }
}