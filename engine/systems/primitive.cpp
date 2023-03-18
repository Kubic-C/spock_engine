#include "primitive.hpp"
#include "physics.hpp"
#include "core/internal.hpp"
#include "render/renderer.hpp"

namespace spk {
    void primitive_render_init(flecs::world& world) {
        spk_trace();

        world.system<spk::comp_rigid_body_t>().
    }
}