#pragma once

#include "systems.hpp"
#include "systems/window.hpp"
#include "systems/physics.hpp"
#include "systems/render.hpp"
#include "systems/primitive_render.hpp"
#include "systems/ps_tracker.hpp"
#include "systems/ui.hpp"

#define _SPK_NO_DEF_STATE
#include "state.hpp"
#undef _SPK_NO_DEF_STATE

namespace spk {
    class engine_t {
    public: 
        void init();
        int run();
        void free();

        void print_debug_stats();

        state_t& get_state();

        void set_current_window_size(int w, int h);
        glm::ivec2 get_current_window_size();
        void get_current_window_size(int& w, int& h);
        void set_current_window_title(const std::string& title);

        b2World* get_current_b2World();

        void set_target_fps(double target_fps);
        void set_vsync_opt(vsync_setting_e option);

        flecs::world world;
        system_ctx_allocater_t ctx_alloc;
    };
}