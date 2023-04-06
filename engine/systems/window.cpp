/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "window.hpp"
#include "core/internal.hpp"

namespace spk {
    void window_cs_init(const flecs::world& world) {
        spk_trace();
        
        world.system().kind(on_render_begin_id).iter(window_t::update).add<tag_renderer_t>();
    }
}