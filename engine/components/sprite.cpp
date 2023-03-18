/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "sprite.hpp"

namespace spk {
    void sprite_comp_init(flecs::world& world) {
        spk_trace();
        
        spk_register_component(world, comp_sprite_t);
    }
}