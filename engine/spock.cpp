#include "spock.hpp"

namespace spk {
    extern state_t state;
    extern stats_t stats;

    void engine_t::init() {
        spk_trace();

        SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME, "[emt][red] ENGINE INIT [reset][emt]");

        state.engine = this;

        { // SDL
            if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) < 0) {
                log.log(LOG_TYPE_ERROR, "could not load SDL2 video. %s", SDL_GetError());
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
        }

        { // pipeline
            init_phases(world);

            flecs::entity render_pipeline = world.pipeline()
                .with(flecs::System)
                    .with(flecs::Phase).cascade(flecs::DependsOn)
                    .without(flecs::Disabled).up(flecs::DependsOn)
                    .without(flecs::Disabled).up(flecs::ChildOf)
                .with<tag_render_system_t>().build();

            state.set_current_render_pipeline(render_pipeline);

            flecs::entity game_pipeline = world.pipeline()
                .with(flecs::System)
                    .with(flecs::Phase).cascade(flecs::DependsOn)
                    .without(flecs::Disabled).up(flecs::DependsOn)
                    .without(flecs::Disabled).up(flecs::ChildOf) 
                .without<tag_render_system_t>()
                .without<tag_renderer_t>().build();

            state.set_current_game_pipeline(game_pipeline);
        }

        { // engine setup and state setup
            ctx_alloc.init();
            
            state.set_current_event_system(world.entity().add<tag_events_t>());
            ps_tracker_system_init(ctx_alloc, world);
            window_cs_init(ctx_alloc, world);
            physics_cs_init(ctx_alloc, world);

            // renderers need a working Gl context, so we initialize them after we add a window with a 
            // working GL context
            world.entity().add<comp_window_t>().add<tag_current_window_t>();

            camera_cs_init(ctx_alloc, world);
            render_cs_init(ctx_alloc, world);

            // render systems
            sprite_cs_init(ctx_alloc, world);
            primitive_render_cs_init(ctx_alloc, world);
            ui_cs_init(ctx_alloc, world);

            world.entity().add<ui_comp_canvas_t>().add<ui_tag_current_canvas_t>();
        }

        SPK_DEBUG_EXPR(print_deps_versions());
    }

    int engine_t::run() {
        SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME, "[emt, red] ENGINE RUN [reset, emt]");

        double frame_time = 0.0;
        double delta_time = 0.0;

        double ticks_to_do  = 0.0;
        double last_frame   = 0.0;
        double last_tick    = 0.0;
        double frames_to_do = 0.0;

        // render pipeline contains Window events 
        world.run_pipeline(state.get_current_render_pipeline(), frame_time);
        world.run_pipeline(state.get_current_game_pipeline(), delta_time);

        while(!state.is_exit()) {
            double current_frame = spk::time.get_time();
            frame_time = current_frame - last_frame;
            ticks_to_do += frame_time * state.get_target_tps(false);
            last_frame = current_frame;

            while(ticks_to_do >= 1.0 && !state.is_exit()) {
                ticks_to_do--;

                double current_tick = spk::time.get_time();
                delta_time = current_tick - last_tick;
                last_tick = current_tick;

                stats.delta_time = delta_time;

                // one tick
               world.frame_begin(0.0);
                if(user_state.tick)
                    user_state.tick(*this);
                world.run_pipeline(state.get_current_game_pipeline(), delta_time);
                world.frame_end();
            }

            // when there is tps lag, frame_time will be longer as well
            // in doing so, frames_to_do will also be longer.
            // We dont need to render more than once per tick reasonably,
            // so we only render once when frames_to_do exceeds 1.0
            // even though it may be more then 2.0 (telling us to render two or more times)
            frames_to_do += frame_time * state.get_target_fps(false);
            if(frames_to_do >= 1.0f) {
                frames_to_do = 0.0f;

                if(user_state.update)
                    user_state.update(*this);

                world.run_pipeline(state.get_current_render_pipeline(), frame_time);
                stats.frame_time = frame_time;
            }

            // exit conditions:
            if(world.should_quit())
                state.exit(0);
        }

        log.log(spk::LOG_TYPE_INFO, "exiting with code; %i", state.get_exit_code());

        return state.get_exit_code();
    }

    void engine_t::free() {
        SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME, "[emt, red] ENGINE FREE [reset, emt]");  
        
        SDL_Quit();
        ctx_alloc.free();
    }

    const state_t& engine_t::get_state() {
        return state;
    }


    const stats_t engine_t::get_stats() {
        return stats;
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

    b2World* engine_t::get_current_physics_world() {
        return state.get_current_physics_world();
    }

    void engine_t::set_target_fps(double target_fps) {
        state.set_target_fps(target_fps);
    }

    void engine_t::set_target_tps(double target_tps) {
        state.set_target_tps(target_tps);
    }

    void engine_t::set_vsync_opt(vsync_setting_e option) {
        spk_assert(-1 <= option && option <= 1 && "must be a valid vsync option!");

        state.set_vsync_option(option);
        SDL_GL_SetSwapInterval(option);
    }

    void engine_t::print_deps_versions() {
        SDL_version sdl_ver;
        const unsigned char* ogl_ver;

        SDL_GetVersion(&sdl_ver);
        ogl_ver = glGetString(GL_VERSION);

        log.log(spk::LOG_TYPE_INFO, "OGL Version %s", ogl_ver); 
        log.log(spk::LOG_TYPE_INFO, "SDL Version %u.%u.%u", sdl_ver.major, sdl_ver.minor, sdl_ver.patch);
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