#include "components.hpp"

namespace spk {
    void component_cs_init(flecs::world& world) {
        world.system().with<tag_should_remove_t>().each([&](flecs::entity entity){
            entity.destruct();
        });
    }
}