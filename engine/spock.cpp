#include "spock.hpp"

namespace spk {
    extern state_t state;
    extern stats_t stats;

    void engine_t::init() {
        SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME, "[emt][red] ENGINE INIT [reset][emt]");

        state.engine = this;

        ctx_alloc.init(world);
        //world.set_target_fps(state.get_target_tps(false));

        if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) < 0) {
            log.log(spk::LOG_TYPE_ERROR, "could not load SDL2 video. %s", SDL_GetError());
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        SPK_DEBUG_EXPR({
            int code;
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &code);
            code |= SDL_GL_CONTEXT_DEBUG_FLAG;
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, code);
        })

        { // pipeline
            init_phases(world);

            flecs::entity render_pipeline = world.pipeline()
                .with(flecs::System)
                    .with(flecs::Phase).cascade(flecs::DependsOn)
                    .without(flecs::Disabled).up(flecs::DependsOn)
                    .without(flecs::Disabled).up(flecs::ChildOf)
                .with<render_system_t>().build();

            state.set_current_render_pipeline(render_pipeline);

            flecs::entity game_pipeline = world.pipeline()
                .with(flecs::System)
                    .with(flecs::Phase).cascade(flecs::DependsOn)
                    .without(flecs::Disabled).up(flecs::DependsOn)
                    .without(flecs::Disabled).up(flecs::ChildOf) 
                .without<render_system_t>().build();

            state.set_current_game_pipeline(game_pipeline);
        }

        { // engine setup and state setup
            state.set_current_event_system(world.entity().add<tag_events_t>());
            ps_tracker_system_init(ctx_alloc, world);
            window_cs_init(ctx_alloc, world);
            physics_cs_init(ctx_alloc, world);

            // renderers need a working Gl context, so we initialize them after we add a window with a 
            // working GL context
            world.entity().add<comp_window_t>().add<tag_current_window_t>();
            world.entity().add<comp_b2World_t>().add<tag_current_box2d_world_t>();

            camera_cs_init(ctx_alloc, world);
            render_cs_init(ctx_alloc, world);

            // render systems
            sprite_render_cs_init(ctx_alloc, world);
            primitive_render_cs_init(ctx_alloc, world);
            ui_cs_init(ctx_alloc, world);
            rsrc_mng.init();

            world.entity().add<ui_comp_canvas_t>().add<ui_tag_current_canvas_t>();

            state.get_current_window().get_mut<comp_window_t>()->force_resize_event();
        }

        SPK_DEBUG_EXPR(print_debug_stats());
    }

    int engine_t::run() {
        SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME, "[emt, red] ENGINE RUN [reset, emt]");

        stats.set_ps_stats(true);

        double frame_time = 0.0;
        double delta_time = 0.0;

        double ticks_to_do = 0.0;
        double last_frame = 0.0;
        double last_tick = 0.0;

        while(!state.is_exit()) {
            double current_frame = spk::time.get_time();
            frame_time = current_frame - last_frame;

            // frame_time is (basically) how often a render occurs
            // determing the amount of ticks is simple.
            // if the ticks per seconds is 60, and if the frame_time is 0.1, 
            // 60 * 0.1 is 6 ticks. because the amount of ticks that can
            // occur in 0.1 seconds is 6. Math proof: ~0.0166 * 6 = 0.1,
            // 1 / 60, 1 repersents the time in seconds, and 60 is the amount of ticks.
            // 1 / 60 repersents in seconds the amount of one tick which is ~0.0166
            ticks_to_do += frame_time * state.get_target_tps(false);
            last_frame = current_frame;

            while(ticks_to_do >= 1.0 && !state.is_exit()) {
                ticks_to_do--;

                double current_tick = spk::time.get_time();
                delta_time = current_tick - last_tick;
                last_tick = current_tick;

                stats.set_delta_time(delta_time);

                // one tick
               world.frame_begin(0.0);
                if(user_state.tick)
                    user_state.tick(*this);
                world.run_pipeline(state.get_current_game_pipeline(), delta_time);
                world.frame_end();
            }

            // one update
            if(user_state.update)
                user_state.update(*this);

            world.run_pipeline(state.get_current_render_pipeline(), frame_time);

            // exit conditions:
            if(world.should_quit())
                state.exit(0);
        }

        log.log(spk::LOG_TYPE_INFO, "exiting with code; %i", state.get_exit_code());

        return state.get_exit_code();
    }

    void engine_t::free() {
        SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME, "[emt, red] ENGINE FREE [reset, emt]");  
        
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
        state.set_target_fps(target_fps);
    }

    void engine_t::set_target_tps(double target_tps) {
        state.set_target_tps(target_tps);
        world.set_target_fps(target_tps);
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

        log.log(spk::LOG_TYPE_INFO, "SDL Version %u.%u.%u", sdl_ver.major, sdl_ver.minor, sdl_ver.patch);
        log.log(spk::LOG_TYPE_INFO, "OGL Version %s", ogl_ver);
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

    flecs::ref<comp_camera_t> engine_t::get_current_camera() {
        return state.get_current_camera().get_ref<comp_camera_t>();
    }

    void engine_t::set_current_camera(flecs::entity e) {
        e.add<tag_current_camera_t>();
        state.set_current_camera(e);
    }


    bool engine_t::is_pressed(SDL_Scancode sc) {
        return state.keys[sc];
    }
}