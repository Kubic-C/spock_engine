#pragma once

#include "systems/window.hpp"
#include "systems/physics.hpp"
#include "systems/ps_tracker.hpp"
#include "systems/ui.hpp"
#include "systems/render.hpp"
#include "systems/primitive_render.hpp"
#include "systems/sprite_render.hpp"
#include "systems/camera.hpp"

#include "utility/entity.hpp"

#define _SPK_NO_DEF_STATE
#include "state.hpp"
#undef _SPK_NO_DEF_STATE

namespace spk {
    typedef std::function<void(engine_t& engine)> user_tick_t;
    typedef std::function<void(engine_t& engine)> user_update_t;
    
    struct user_state_t {
        void* user_data      = nullptr;
        user_tick_t tick     = nullptr;
        user_update_t update = nullptr;
    };

    class engine_t {
    public: 
        void init();
        int run();
        void free();

        // outputs the version (if able to) of every library
        // that spock is dependent on 
        void print_debug_stats();

        // state
        const state_t& get_state();

        // sets the current size of the window 
        void set_current_window_size(int w, int h);

        // gets the current size of the window in ivec2 format 
        glm::ivec2 get_current_window_size();

        // gets the current size of the window by using output parameters
        void get_current_window_size(int& w, int& h);

        // sets the title of the window
        void set_current_window_title(const std::string& title);

        // gets the box2d physics world currently in use 
        b2World* get_current_b2World();
        
        // sets the target FPS in flecs
        void set_target_fps(double target_fps);

        // sets the vertical sync (V-sync) type
        void set_vsync_opt(vsync_setting_e option);
        
        // sets pixels per meter
        void set_ppm(float ppm);

        // gets ppm
        float get_ppm();

        // signal the engine to end
        void exit(int code);

        flecs::world world;
        system_ctx_allocater_t ctx_alloc;
        resource_manager_t rsrc_mng;
        user_state_t user_state;
    };
}