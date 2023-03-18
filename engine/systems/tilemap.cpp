/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "tilemap.hpp"
#include "core/internal.hpp"
#include "components/rigid_body.hpp"
#include "sprite.hpp"

namespace spk {

    void tilemap_cs_init(flecs::world& world) {
        spk_trace();
        
        tile_comp_init(world);
    }
}