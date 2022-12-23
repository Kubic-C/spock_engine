#pragma once

#include "framework.hpp"

/* global accessible state | acting like a global context */

namespace spk {
    class engine_t; // forward decloration

    enum vsync_setting_e {
        VSYNC_ADAPATIVE = -1,
        VSYNC_DISABLED = 0,
        VSYNC_ENABLED = 1
    };

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

    public:
        engine_t* engine;

        void exit(int exit_code) { exit_ = true; exit_code_ = 0; }
        int get_exit_code() { return exit_code_; }
        bool is_exit() { return exit_; }

        void set_ppm(float ppm) { this->ppm = ppm; };
        float get_ppm() const { return ppm; }

        void set_target_fps(double fps) { target_fps_ = fps; }
        double get_target_fps(bool divide_by_second = true) const;
        void set_target_tps(double tps) { target_tps_ = tps; }
        double get_target_tps(bool divide_by_second = true) const;

        void set_current_window(const flecs::entity& window_) { window = window_; }
        flecs::entity get_current_window() const { return window; }

        void set_current_box2D_world(const flecs::entity& world) { box2D_world = world; }
        flecs::entity get_current_box2D_world() const { return box2D_world; }

        void set_current_renderer(const flecs::entity& renderer_) { renderer = renderer_; }
        flecs::entity get_current_renderer() const { return renderer; }

        void set_vsync_option(vsync_setting_e option) { vsync_opt = option; }
        vsync_setting_e get_vsync_option() const { return vsync_opt; }

        void set_current_event_system(flecs::entity event_sys) { event_system = event_sys; }
        flecs::entity get_current_event_system() const { return event_system; }

        void set_current_canvas(flecs::entity canvas_) { canvas = canvas_; }
        flecs::entity get_current_canvas() const { return canvas; }; 
    };

    struct stats_t {
    private:
        bool print_ps_stats = true;
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