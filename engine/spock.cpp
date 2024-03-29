/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

    refer to license: https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "spock.hpp"
#include "core/internal.hpp"

#ifdef NDEBUG
#define MODE "RELEASE"
#else 
#define MODE "DEBUG"
#endif 

#define STR(x) #x
#define XSTR(x) STR(x)

namespace spk {
    void vsync_set(vsync_setting_e vsync_opt) {
        spk::settings().vsync_opt = vsync_opt;
        SDL_GL_SetSwapInterval(vsync_opt);
    }

    const char* build_name() {
        return MODE " build " XSTR(SPK_MAJOR) "." XSTR(SPK_MINOR) "." XSTR(SPK_PATCH);
    }

    void init_SDL2() {
        const int audio_flags = MIX_INIT_MP3;

        SDL_SetMainReady();

        if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            log.log(LOG_TYPE_ERROR, "could not load SDL2 video. %s", SDL_GetError());
        }

        // rendering

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        SPK_DEBUG_EXPR({
            int code;
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &code);
            code |= SDL_GL_CONTEXT_DEBUG_FLAG;
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, code);
        })

        // audio
        if(Mix_Init(audio_flags) != audio_flags) {
            log.log(LOG_TYPE_ERROR, "could not load SDL2_Mixer with flags %i", audio_flags);
        }

        audio_open();
    }

    void init_defualt_ecs_pipelines() {
        init_phases(ecs_world());

        flecs::entity render_pipeline = ecs_world().pipeline()
            .with(flecs::System)
                .with(flecs::Phase).cascade(flecs::DependsOn)
                .without(flecs::Disabled).up(flecs::DependsOn)
                .without(flecs::Disabled).up(flecs::ChildOf)
            .with<tag_renderer_t>().build();

        internal->scene.render_pipeline = render_pipeline;

        flecs::entity game_pipeline = ecs_world().pipeline()
            .with(flecs::System)
                .with(flecs::Phase).cascade(flecs::DependsOn)
                .without(flecs::Disabled).up(flecs::DependsOn)
                .without(flecs::Disabled).up(flecs::ChildOf) 
            .without<tag_mesh_t>()
            .without<tag_renderer_t>().build();

        internal->scene.game_pipeline = game_pipeline;
    }

    void init() {
        spk_trace();
        SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME, "[emt][red] ENGINE INIT [reset][emt]");

        init_SDL2();

        internal = new internal_t;
        internal->settings.log_statistics = true;

        font_library_init();
        init_defualt_ecs_pipelines();

        { // creating default scene
            window_t*         window   = scene().stack.push<window_t>();
            canvas_t*         canvas   = scene().stack.push<canvas_t>();
            render_context_t* renderer = nullptr;

            window_make_current(*window);
            canvas_make_current(*canvas);

            // creating the renderer, MUST come after creating and making a window
            // current. As we must need a valid OpenGL context
            renderer = scene().stack.push<render_context_t>(); 
            render_context_make_current(*renderer);
        }

        { // engine setup and state setup            
            internal->scene.event_system = ecs_world().entity().add<tag_events_t>();

            // misc
            component_cs_init(ecs_world());
            window_cs_init(ecs_world());
            ps_tracker_system_init(ecs_world());

            // game components
            character_controller_cs_init(ecs_world());
            camera_cs_init(ecs_world());
            physics_cs_init(ecs_world());
            tilemap_cs_init(ecs_world());
            particles_cs_init(ecs_world());

            // render systems
            render_system_init(ecs_world());
            sprite_cs_init(ecs_world());
            primitive_render_init(ecs_world());
            ui_cs_init(ecs_world());
        }
    }

    int run() {
        spk_trace();

        SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME, "[emt, red] ENGINE RUN [reset, emt]");

        double delta_time = 0.0;   // time between ticks
        double last_tick    = 0.0; // time of last tick
        double ticks_to_do  = 0.0; // the amount of ticks to do

        double frame_time = 0.0;   // time between render frames
        double last_frame   = 0.0; // time of last frame
        double frames_to_do = 0.0; // the amount of frames to do

        // render pipeline contains Window events 
        ecs_world().run_pipeline(internal->scene.render_pipeline, frame_time);
        ecs_world().run_pipeline(internal->scene.game_pipeline, delta_time);

        while(!internal->settings.should_exit) {
            double current_frame = spk::time.get_time();
            frame_time = current_frame - last_frame;
            ticks_to_do += frame_time * internal->settings.target_tps;
            last_frame = current_frame;

            while(ticks_to_do >= 1.0 && !internal->settings.should_exit) {
                ticks_to_do--;

                double current_tick = spk::time.get_time();
                delta_time = current_tick - last_tick;
                last_tick = current_tick;

                internal->statistics.delta_time = delta_time;

                // one tick
               ecs_world().frame_begin(0.0);
                if(internal->scene.user_data.tick)
                    internal->scene.user_data.tick();
                ecs_world().run_pipeline(internal->scene.game_pipeline, delta_time);
                ecs_world().frame_end();
            }

            // when there is tps lag, frame_time will be longer as well
            // in doing so, frames_to_do will also be longer.
            // We dont need to render more than once per tick reasonably,
            // so we only render once when frames_to_do exceeds 1.0
            // even though it may be more then 2.0 (telling us to render two or more times)
            frames_to_do += frame_time * internal->settings.target_fps;
            if(frames_to_do >= 1.0f) {
                frames_to_do = 0.0f;

                if(internal->scene.user_data.update)
                    internal->scene.user_data.update();

                ecs_world().run_pipeline(internal->scene.render_pipeline, frame_time);
                internal->statistics.frame_time = frame_time;
            }

            // exit conditions:
            if(ecs_world().should_quit())
                internal->settings.should_exit = true;
        }

        return internal->exit_info.code;
    }

    void free() {
        spk_trace();

        SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME, "[emt, red] ENGINE FREE [reset, emt]");  

        font_library_free();
        delete internal;

        Mix_Quit();
        SDL_Quit();
    }

    void print_deps_versions() {
        int ft_major, ft_minor, ft_patch;
        SDL_version sdl_version;
        SDL_version mixer_version;
        const unsigned char* ogl_ver;

        FT_Library_Version(resources().ft_lib, &ft_major, &ft_minor, &ft_patch);
        SDL_GetVersion(&sdl_version);
        MIX_VERSION(&mixer_version);
        ogl_ver = glGetString(GL_VERSION);

        log.log("FreeType Version: %i.%i.%i", ft_major, ft_minor, ft_patch);
        log.log("OGL Version: %s", ogl_ver); 
        log.log("SDL Version: %u.%u.%u", sdl_version.major, sdl_version.minor, sdl_version.patch);
        log.log("SDL_mixer Version: %u.%u.%u", mixer_version.major, mixer_version.minor, mixer_version.patch);
    }

    void exit() {
        internal->settings.should_exit = true;
    }
}