#include "camera.hpp"
#include "events.hpp"
#include "state.hpp"

namespace spk {
    void camera_resize(flecs::iter& iter) {
        auto ctx = SPK_GET_CTX(iter, camera_system_ctx_t);
        auto camera = state.get_current_camera().get_ref<comp_camera_t>();    
        event_window_size_t* size = iter.param<event_window_size_t>();

        camera->size = { static_cast<float>(size->width), static_cast<float>(size->height) };
    }

    void camera_system_update(comp_camera_t& camera) {
        camera.recalculate();
    }

    void camera_cs_init(system_ctx_allocater_t& allocater, flecs::world& world) {
        camera_comp_init(world);
        
        auto ctx = allocater.allocate_ctx<camera_system_ctx_t>();

        world.entity().add<comp_camera_t>().add<tag_current_camera_t>(); // the default camera

        world.system<comp_camera_t>().term<tag_current_camera_t>().kind(on_render_begin)
            .each(camera_system_update).add<tag_render_system_t>();

        world.observer().term<tag_events_t>().event<event_window_size_t>().ctx(ctx)
            .iter(camera_resize);
    }
}