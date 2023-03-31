/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "camera.hpp"
#include "core/internal.hpp"

namespace spk {
    void camera_resize(flecs::iter& iter) {
        auto                 camera = internal->scene.camera.get_ref<comp_camera_t>();    
        event_window_size_t* size   = iter.param<event_window_size_t>();

        camera->size = { static_cast<float>(size->width), static_cast<float>(size->height) };
    }

    void camera_system_update(comp_camera_t& camera) {
        camera.recalculate();
    }

    void camera_cs_init(flecs::world& world) {
        spk_trace();
        
        camera_comp_init(world);

        world.entity().add<comp_camera_t>().add<tag_current_camera_t>(); // the default camera

        world.system<comp_camera_t>().term<tag_current_camera_t>().kind(on_render_begin_id)
            .each(camera_system_update).add<tag_renderer_t>();

        world.observer().term<tag_events_t>().event<event_window_size_t>().iter(camera_resize);
    }
}