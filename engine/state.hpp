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

        flecs::entity window = flecs::entity(UINT64_MAX);
        flecs::entity box2D_world = flecs::entity(UINT64_MAX);
        flecs::entity renderer = flecs::entity(UINT64_MAX);
        flecs::entity event_system = flecs::entity(UINT64_MAX);

    public:
        engine_t* engine;

        void exit(int exit_code) { exit_ = true; exit_code_ = 0; }
        int get_exit_code() { return exit_code_; }
        bool is_exit() { return exit_; }

        void _set_target_fps(double fps) { target_fps_ = fps; }
        double _get_target_fps(bool divide_by_second = true);
        void _set_target_tps(double tps) { target_tps_ = tps; }
        double _get_target_tps(bool divide_by_second = true);

        void _set_current_window(const flecs::entity& window_) { window = window_; }
        flecs::entity _get_current_window() { return window; }

        void _set_current_box2D_world(const flecs::entity& world) { box2D_world = world; }
        flecs::entity _get_current_box2D_world() { return box2D_world; }

        void _set_current_renderer(const flecs::entity& renderer_) { renderer = renderer_; }
        flecs::entity _get_current_renderer() { return renderer; }

        void _set_vsync_option(vsync_setting_e option) { vsync_opt = option; }
        vsync_setting_e _get_vsync_option() { return vsync_opt; }

        void _set_current_event_system(flecs::entity event_sys) { event_system = event_sys; }
        flecs::entity _get_current_event_system() { return event_system; }
    };

#ifndef _SPK_NO_DEF_STATE
    extern state_t state;
#endif
};