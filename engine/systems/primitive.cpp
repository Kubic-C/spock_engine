#include "primitive.hpp"
#include "physics.hpp"
#include "core/internal.hpp"
#include "render/render_system.hpp"

namespace spk {
    void primitive_render_init(flecs::world& world) {
        spk_trace();
        
        auto primitive_renderer = internal->allocators.stack.push<primitive_renderer_t>();

        render_system().rp_add_renderer(0, (base_renderer_t*)primitive_renderer);
    }
}