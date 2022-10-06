#pragma once
#include "systems/renderer.hpp"
#include "systems/physics.hpp" 
#include "systems/ui.hpp"

namespace spk {
    void print_name2();

    class engine_t {
    public:
        scene_t* scene;
        sfk::window_t window;

        engine_t();

        void init(int w, int h, std::string title);
        void free();

        void push_system(system_t* system);
        void push_render_system(render_system_t* system); // this will not init the system

        void set_time_exit(int ticks_till_end) { time.exit = ticks_till_end; };
        void set_tick_speed(int tick_speed);
        float get_tick_speed() const { return time.fps_limiter; }
        void update();
        void loop();

        float get_elapsed_time();
        float get_time() { return sfk::time.get_time(); };

        resource_manager_t resource_manager;

    private:
        sfk::sfk_t framework;
        renderer2D_t renderer;
        physics2D_t physics;
        ui_t ui;

        system_manager_t system_manager;

        struct {
            int exit = INT_MAX;
            int ticks = 0, frames = 0;
            int ticks_per_second  = 0;
            double fps_limiter    = 0.0; 
            double delta          = 0.0;
            double ticks_to_do    = 0.0; 
            double last_frame     = 0.0;
            double second_timer   = 0.0; 
        } time;
    };

}