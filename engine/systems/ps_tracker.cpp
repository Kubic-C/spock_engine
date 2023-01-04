#include "ps_tracker.hpp"
#include "state.hpp"

namespace spk {
    void ps_tracker_system_tick(flecs::iter& iter) {
        auto ctx = get_ctx<ps_tracker_ctx_t>(iter);

        ctx->tps++;
        ctx->average_delta_time += time.get_time() - ctx->last_tick;
        ctx->last_tick = time.get_time();
    }

    void ps_tracker_system_update(flecs::iter& iter) {
        auto ctx = get_ctx<ps_tracker_ctx_t>(iter);
        
        ctx->fps++;
    }

    void ps_tracker_system_update_sec(flecs::iter& iter) {
        auto ctx = get_ctx<ps_tracker_ctx_t>(iter);

        ctx->average_delta_time /= ctx->tps;

        if(stats.print_ps_stats) {
            log.log(spk::LOG_TYPE_INFO, "FPS: %u | TPS: %u | DELTA-TIME: %f", ctx->fps, ctx->tps, ctx->average_delta_time);
        }

        stats.fps = ctx->fps;
        stats.tps = ctx->tps;
        stats.average_delta_time = ctx->average_delta_time;

        ctx->fps = 0;
        ctx->tps = 0;
        ctx->average_delta_time = 0;
    }

    void ps_tracker_system_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        auto ctx = ctx_alloc.allocate_ctx<ps_tracker_ctx_t>();

        world.system().ctx(ctx).iter(ps_tracker_system_tick);
        world.system().ctx(ctx).kind(on_render).iter(ps_tracker_system_update).add<tag_render_system_t>();
        world.system().ctx(ctx).interval(1.0).iter(ps_tracker_system_update_sec);
    }
}