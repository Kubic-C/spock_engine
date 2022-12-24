#pragma once

#include "base.hpp"

/* global accessible state | acting like a global context */

namespace spk {
    class engine_t; // forward decloration

    enum vsync_setting_e {
        VSYNC_ADAPATIVE = -1,
        VSYNC_DISABLED = 0,
        VSYNC_ENABLED = 1,
        VSYNC_UKNOWN = 0xFFFF
    };

    const char* vsync_name(vsync_setting_e opt);

    // functions labeled with an '_' means changing the option will not 
    // have an affect on the current working state. Change the setting through
    // engine_t will have an immediate affect
    class state_t {
    private:
        bool exit_ = false;
        int exit_code_ = 0;
        double target_fps_ = 120.0;
        double target_tps_ = 60.0;
        vsync_setting_e vsync_opt = VSYNC_DISABLED; 
        float ppm = 8.0f;

        flecs::entity window = flecs::entity(UINT64_MAX);
        flecs::entity box2D_world = flecs::entity(UINT64_MAX);
        flecs::entity renderer = flecs::entity(UINT64_MAX);
        flecs::entity event_system = flecs::entity(UINT64_MAX);
        flecs::entity canvas = flecs::entity(UINT64_MAX);
        flecs::entity camera = flecs::entity(UINT64_MAX);

    public:
        engine_t* engine;

        void exit(int exit_code) { exit_ = true; exit_code_ = 0; }
        int get_exit_code() { return exit_code_; }
        bool is_exit() { return exit_; }

        float get_ppm() const { return ppm; }
        double get_target_fps(bool divide_by_second = true) const;
        double get_target_tps(bool divide_by_second = true) const;
        flecs::entity get_current_window() const { return window; }
        flecs::entity get_current_box2D_world() const { return box2D_world; }
        flecs::entity get_current_renderer() const { return renderer; }
        vsync_setting_e get_vsync_option() const { return vsync_opt; }
        flecs::entity get_current_event_system() const { return event_system; }
        flecs::entity get_current_canvas() const { return canvas; }; 
        flecs::entity get_current_camera() const { return camera; }; 

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

        void set_current_box2D_world(flecs::entity world) { 
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "current box2D world changed to: (id)%llu", world.id());
            box2D_world = world; 
        }

        void set_current_renderer(flecs::entity renderer_) {             
            SPK_DEBUG_LOG_IF(DEBUG_FLAGS_ENABLE_STATE_CHANGE, "current renderer changed to: (id)%llu", renderer_.id());
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
    };

    struct stats_t {
    private:
        bool print_ps_stats = false;
        uint32_t fps;
        uint32_t tps;
    
    public:
        void set_ps_stats(bool print_per_second) { print_ps_stats = print_per_second; }
        bool get_ps_stats() { return print_ps_stats; }

        void _set_fps(uint32_t fps_) { fps = fps_; };
        void _set_tps(uint32_t tps_) { fps = tps_; };
        uint32_t get_fps() { return fps; }
        uint32_t get_tps() { return tps; }

    };

#ifndef _SPK_NO_DEF_STATE
    extern state_t state;
    extern stats_t stats;
#endif
};