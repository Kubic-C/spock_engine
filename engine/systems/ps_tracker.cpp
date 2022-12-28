#include "ps_tracker.hpp"
#include "state.hpp"

namespace spk {
    void ps_tracker_system_tick(ps_tracker_ctx_t& ctx) {
        ctx.tps++;
    }

    void ps_tracker_system_update(ps_tracker_ctx_t& ctx) {
        ctx.fps++;
    }

    void ps_tracker_system_update_sec(flecs::iter& iter, ps_tracker_ctx_t* ctx) {
        // NOTE: ctx is a pointer to an array

        if(stats.get_ps_stats()) {
            log.log(spk::LOG_TYPE_INFO, "FPS: %u | TPS: %u | DELTA-TIME: %f", ctx->fps, ctx->tps, stats.get_delta_time());
        }

        stats.set_fps(ctx->fps);
        stats.set_tps(ctx->tps);

        ctx->fps = 0;
        ctx->tps = 0;
    }

    void ps_tracker_system_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        auto ctx = ctx_alloc.allocate_ctx<ps_tracker_ctx_t>();

        world.system<ps_tracker_ctx_t>().each(ps_tracker_system_tick);
        world.system<ps_tracker_ctx_t>().kind(on_render).each(ps_tracker_system_update).add<render_system_t>();
        world.system<ps_tracker_ctx_t>().interval(1.0).iter(ps_tracker_system_update_sec);
    }
}