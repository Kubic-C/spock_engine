#include "spock.hpp"

namespace spk {
    extern state_t state;

    void engine_t::init() {
        state.engine = this;

        ctx_alloc.init(world);
        world.set_target_fps(state._get_target_fps(false));
        state._set_current_event_system(world.entity().add<tag_events_t>());

        if(SDL_Init(SDL_INIT_VIDEO) < 0) {
            sfk::log.log(sfk::LOG_TYPE_ERROR, "could not load SDL2 video. %s", SDL_GetError());
        }

        ps_tracker_system_init(ctx_alloc, world);
        window_cs_init(ctx_alloc, world);
        physics_cs_init(ctx_alloc, world);

        // renderers need a working Gl context, so we initialize them after we add a window with a 
        // working GL context
        world.entity().add<comp_window_t>().add<tag_current_window_t>();
        world.entity().add<comp_box2d_world_t>().add<tag_current_box2d_world_t>();

        collider_render_cs_init(ctx_alloc, world);
        render_cs_init(ctx_alloc, world);

        state._get_current_window().get_mut<comp_window_t>()->force_resize_event();
    }

    int engine_t::run() {
        while(!state.is_exit() && world.progress(0)) {}

        sfk::log.log(sfk::LOG_TYPE_INFO, "exiting with code; %i", state.get_exit_code());

        return state.get_exit_code();
    }

    void engine_t::free() {
        SDL_Quit();
        ctx_alloc.free();
    }

    state_t& engine_t::get_state() {
        return state;
    }


    void engine_t::set_current_window_size(int w, int h) {
        state._get_current_window().get_mut<comp_window_t>()->set_size(w, h);
    }

    glm::ivec2 engine_t::get_current_window_size() {
        return state._get_current_window().get_mut<comp_window_t>()->get_size();
    }

    void engine_t::get_current_window_size(int& w, int& h) {
        glm::ivec2 size = state._get_current_window().get_mut<comp_window_t>()->get_size();
        w = size.x;
        h = size.y;
    }

    void engine_t::set_current_window_title(const std::string& title) {
        state._get_current_window().get_mut<comp_window_t>()->set_title(title);
    }

    b2World* engine_t::get_current_b2World() {
        return state._get_current_box2D_world().get<comp_box2d_world_t>()->world;
    }

    void engine_t::set_target_fps(double target_fps) {
        world.set_target_fps(target_fps);
        state._set_target_fps(target_fps);
    }

    void engine_t::set_vsync_opt(vsync_setting_e option) {
        sfk_assert(-1 <= option && option <= 1 && "must be a valid vsync option!");

        state._set_vsync_option(option);
        SDL_GL_SetSwapInterval(option);
    }
}