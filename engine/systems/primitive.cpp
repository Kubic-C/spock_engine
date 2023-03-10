#include "primitive.hpp"
#include "state.hpp"
#include "spock.hpp"
#include "physics.hpp"

namespace spk {
    void primitive_render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        spk_trace();
        
        auto primi_renderer = ctx_alloc.allocate_ctx<primitive_renderer_t>();

        state.get_current_renderer()->rp_add_renderer(0, (base_renderer_t*)primi_renderer);
    }
}