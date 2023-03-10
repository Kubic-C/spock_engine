#include "physics.hpp"
#include "state.hpp"

namespace spk {
    class physics_system_t : public spk::system_t {
    public:
        physics_system_t() {
            world = new b2World(glm::vec2(0.0f, -9.81f));
        }

        ~physics_system_t() {
            delete world;
        }

        b2World* world;
    };

    void physics_system_tick(flecs::iter iter) {
        auto physics_world = state.get_current_physics_world();
    
        physics_world->Step(stats.delta_time, 8, 3);
    }

    void physics_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        spk_trace();        

        rigid_body_comp_init(world);

        auto ctx = ctx_alloc.allocate_ctx<physics_system_t>();

        world.system().iter(physics_system_tick);

        state.set_current_physics_world(ctx->world);
    }
}