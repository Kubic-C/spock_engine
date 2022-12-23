#include "spock.hpp"

namespace spk {
    extern state_t state;

    void engine_t::init() {
        state.engine = this;

        ctx_alloc.init(world);
        world.set_target_fps(state.get_target_fps(false));
        state.set_current_event_system(world.entity().add<tag_events_t>());

        if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) < 0) {
            sfk::log.log(sfk::LOG_TYPE_ERROR, "could not load SDL2 video. %s", SDL_GetError());
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        DEBUG_EXPR({
            int code;
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &code);
            code |= SDL_GL_CONTEXT_DEBUG_FLAG;
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, code);
        })

        ps_tracker_system_init(ctx_alloc, world);
        window_cs_init(ctx_alloc, world);
        physics_cs_init(ctx_alloc, world);

        // renderers need a working Gl context, so we initialize them after we add a window with a 
        // working GL context
        world.entity().add<comp_window_t>().add<tag_current_window_t>();
        world.entity().add<comp_b2World_t>().add<tag_current_box2d_world_t>();

        render_cs_init(ctx_alloc, world);

        // render systems
        sprite_render_cs_init(ctx_alloc, world);
        primitive_render_cs_init(ctx_alloc, world);
        ui_cs_init(ctx_alloc, world);
        rsrc_mng.init();

        world.entity().add<ui_comp_canvas_t>().add<ui_tag_current_canvas_t>();

        state.get_current_window().get_mut<comp_window_t>()->force_resize_event();

        DEBUG_EXPR(print_debug_stats());
    }

    int engine_t::run() {
        double delta_time = 0.0;
        double ticks_to_do = 0.0;
        double last_frame = 0.0;
        
        while(!state.is_exit()) {
            double current_frame = sfk::time.get_time();
            delta_time = current_frame - last_frame;
            ticks_to_do += delta_time / state.get_target_tps(true);
            last_frame = current_frame;

            if(user_state.tick != nullptr) {
                while(ticks_to_do >= 1.0) {
                    ticks_to_do--;

                    // one tick
                    user_state.tick(*this);
                }
            } else {
                ticks_to_do = 0.0;
            }

            // one update
            world.frame_begin(delta_time);

            world.run_pipeline(flecs::entity(0ULL), delta_time);

            if(user_state.update != nullptr)
                user_state.update(*this);

            world.frame_end();

            // exit conditions:
            if(world.should_quit())
                state.exit(0);
        }

        sfk::log.log(sfk::LOG_TYPE_INFO, "exiting with code; %i", state.get_exit_code());

        return state.get_exit_code();
    }

    void engine_t::free() {
        rsrc_mng.free();
        SDL_Quit();
        ctx_alloc.free();
    }

    const state_t& engine_t::get_state() {
        return state;
    }

    void engine_t::set_current_window_size(int w, int h) {
        state.get_current_window().get_ref<comp_window_t>()->set_size(w, h);
    }

    glm::ivec2 engine_t::get_current_window_size() {
        return state.get_current_window().get_ref<comp_window_t>()->get_size();
    }

    void engine_t::get_current_window_size(int& w, int& h) {
        glm::ivec2 size = state.get_current_window().get_ref<comp_window_t>()->get_size();
        w = size.x;
        h = size.y;
    }

    void engine_t::set_current_window_title(const std::string& title) {
        state.get_current_window().get_ref<comp_window_t>()->set_title(title);
    }

    b2World* engine_t::get_current_b2World() {
        return state.get_current_box2D_world().get<comp_b2World_t>()->world;
    }

    void engine_t::set_target_fps(double target_fps) {
        world.set_target_fps(target_fps);
        state.set_target_fps(target_fps);
    }

    void engine_t::set_vsync_opt(vsync_setting_e option) {
        sfk_assert(-1 <= option && option <= 1 && "must be a valid vsync option!");

        state.set_vsync_option(option);
        SDL_GL_SetSwapInterval(option);
    }

    void engine_t::print_debug_stats() {
        SDL_version sdl_ver;
        const unsigned char* ogl_ver;

        SDL_GetVersion(&sdl_ver);
        ogl_ver = glGetString(GL_VERSION);

        sfk::log.log(sfk::LOG_TYPE_INFO, "SDL Version %u.%u.%u", sdl_ver.major, sdl_ver.minor, sdl_ver.patch);
        sfk::log.log(sfk::LOG_TYPE_INFO, "OGL Version %s", ogl_ver);
    }

    void engine_t::set_ppm(float ppm) {
        state.set_ppm(ppm);
        state.get_current_window().get_ref<comp_window_t>()->force_resize_event();
    }

    float engine_t::get_ppm() {
        return state.get_ppm();
    }

    void engine_t::exit(int code) {
        state.exit(code);
    }
}