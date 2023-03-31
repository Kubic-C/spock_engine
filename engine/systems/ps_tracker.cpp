/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "ps_tracker.hpp"
#include "core/internal.hpp"

namespace spk {
    /* this source-file only variables */
    
    // this changes every frame to count ticks, frames, delta_time etc..
    // unlike whats in internal->statistics which is only updated per second
    struct {
        uint32_t fps = 0;
        uint32_t tps = 0;
        float average_delta_time = 0.0f;
        float last_tick = 0.0f;
    } stats_counter;

    void ps_tracker_system_tick(flecs::iter& iter) {
        stats_counter.tps++;
        stats_counter.average_delta_time += time.get_time() - stats_counter.last_tick;
        stats_counter.last_tick = time.get_time();
    }

    void ps_tracker_system_update(flecs::iter& iter) {
        stats_counter.fps++;
    }

    void ps_tracker_system_update_sec(flecs::iter& iter) {
        stats_counter.average_delta_time /=stats_counter.tps;

        if(internal->settings.log_statistics) {
            log.log(spk::LOG_TYPE_INFO, "FPS: %u | TPS: %u | DELTA-TIME: %f", stats_counter.fps, stats_counter.tps, stats_counter.average_delta_time);
        }

        internal->statistics.fps = stats_counter.fps;
        internal->statistics.tps = stats_counter.tps;
        internal->statistics.average_delta_time = stats_counter.average_delta_time;

        stats_counter.fps = 0;
        stats_counter.tps = 0;
        stats_counter.average_delta_time = 0;
    }

    void ps_tracker_system_init(flecs::world& world) {
        spk_trace();

        world.system().iter(ps_tracker_system_tick);
        world.system().kind(on_render_id).iter(ps_tracker_system_update).add<tag_renderer_t>();
        world.system().interval(1.0).iter(ps_tracker_system_update_sec);
    }
}