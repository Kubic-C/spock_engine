/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "components.hpp"

namespace spk {
    void component_cs_init(const flecs::world& world) {
        world.system().with<tag_should_remove_t>().each([&](flecs::entity entity){
            entity.destruct();
        });
    }
}