#include "components.hpp"

namespace spk {
    void component_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        world.system().with<tag_should_remove_t>().each([&](flecs::entity entity){
            entity.destruct();
        });
    }
}