#pragma once

#include "base.hpp"
#include "systems.hpp"

/* global accessible state | acting like a global context */

namespace spk {
    class engine_t; // forward decloration
    class render_system_t;
    class physics_world_t;

    enum vsync_setting_e {
        VSYNC_ADAPATIVE = -1,
        VSYNC_DISABLED = 0,
        VSYNC_ENABLED = 1,
        VSYNC_UKNOWN = 0xFFFF
    };

    const char* vsync_name(vsync_setting_e opt);

    /* the reason for state_t using getters and setters is because any changes that do
        occur to it should be logged as way to track global-state related bugs */
    class state_t {
    private:
        bool exit_ = false;
        int exit_code_ = 0;
        double target_fps_ = 120.0;
        double target_tps_ = 60.0;
        vsync_setting_e vsync_opt = VSYNC_DISABLED; 
        float ppm = 8.0f;

        flecs::entity    window          = flecs::entity(UINT64_MAX);
        physics_world_t* physics_world   = nullptr;
        render_system_t* renderer        = nullptr;
        flecs::entity    event_system    = flecs::entity(UINT64_MAX);
        flecs::entity    canvas          = flecs::entity(UINT64_MAX);
        flecs::entity    camera          = flecs::entity(UINT64_MAX);
        flecs::entity    render_pipeline = flecs::entity(UINT64_MAX);
        flecs::entity    game_pipeline   = flecs::entity(UINT64_MAX);

    public:
        std::bitset<SDL_NUM_SCANCODES> keys;
        engine_t* engine;

        void exit(int exit_code) { exit_ = true; exit_code_ = 0; }
        int get_exit_code() { return exit_code_; }
        bool is_exit() { return exit_; }

        float            get_ppm() const { return ppm; }
        double           get_target_fps(bool divide_by_second = true) const;
        double           get_target_tps(bool divide_by_second = true) const;
        flecs::entity    get_current_window() const { return window; }
        physics_world_t* get_current_physics_world() const { return physics_world; }
        render_system_t* get_current_renderer() const { return renderer; }
        vsync_setting_e  get_vsync_option() const { return vsync_opt; }
        flecs::entity    get_current_event_system() const { return event_system; }
        flecs::entity    get_current_canvas() const { return canvas; }; 
        flecs::entity    get_current_camera() const { return camera; }; 
        flecs::entity    get_current_render_pipeline() const { return render_pipeline; }
        flecs::entity    get_current_game_pipeline() const { return game_pipeline; }

        void set_ppm(float ppm) { 
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "pixels per meter(PPM) changed to: %f", ppm);
            this->ppm = ppm; 
        };

        void set_target_fps(double fps) { 
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "target fps changed to: %f", fps);
            target_fps_ = fps; 
        }
        
        void set_target_tps(double tps) { 
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "target tps changed to: %f", tps);
            target_tps_ = tps; 
        }        

        void set_current_window(flecs::entity window_) { 
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "current window changed to: (id)%llu", window_.id());
            window = window_; 
        }

        void set_current_physics_world(physics_world_t* world) { 
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "current physics world changed to: %p", world);
            physics_world = world; 
        }

        void set_current_renderer(render_system_t* renderer_) {             
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "current renderer changed to: %p", renderer_);
            renderer = renderer_; 
        }

        void set_vsync_option(vsync_setting_e option) { 
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "current VSYNC setting changed to: %s", vsync_name(option))
            vsync_opt = option; 
        }

        void set_current_event_system(flecs::entity event_sys) { 
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "current event system changed to: (id)%llu", event_sys.id());
            event_system = event_sys; 
        }

        void set_current_canvas(flecs::entity canvas_) { 
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "current canvas changed to: (id)%llu", canvas_.id());
            canvas = canvas_; 
        }
     
        void set_current_camera(flecs::entity camera_) {
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "current camera changed to: (id)%llu", camera_.id()); 
            camera = camera_; 
        }

        void set_current_render_pipeline(flecs::entity render_pipeline_) {
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "current render pipeline changed to: (id)%llu", render_pipeline_.id()); 
            render_pipeline = render_pipeline_;
        }
   
        void set_current_game_pipeline(flecs::entity game_pipeline_) {
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "current game pipeline changed to: (id)%llu", game_pipeline_.id()); 
            game_pipeline = game_pipeline_;
        }
    };

    struct stats_t {
        bool     print_ps_stats     = false;
        double   delta_time         = 0.0;
        double   frame_time         = 0.0;
        double   average_delta_time = 0;
        uint32_t fps                = 0;
        uint32_t tps                = 0;
    };

#ifndef _SPK_NO_DEF_STATE
    extern state_t state;
    extern stats_t stats;
#endif
};