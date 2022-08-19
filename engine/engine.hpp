#pragma once
#include "systems/renderer.hpp"
#include "systems/physics.hpp" 
#include "systems/ui.hpp"

namespace spk {
    void print_name2();

    class engine_tt {
    public:
        scene_tt* scene;
        sfk::window_tt window;

        engine_tt();

        void init(int w, int h, std::string title);
        void free();

        void push_system(system_tt* system);

        void set_time_exit(int ticks_till_end) { time.exit = ticks_till_end; };
        void set_tick_speed(int tick_speed);
        float get_tick_speed() const { return time.fps_limiter; }
        void update();
        void loop();

        float get_elapsed_time();
        float get_time() { return framework.get_time(); };

        resource_manager_tt resource_manager;

    private:
        sfk::sfk framework;
        renderer2D_tt renderer;
        physics2D_tt physics;
        ui_tt ui;

        system_manager_tt system_manager;

        struct time_tt {
            int exit = INT_MAX;
            int ticks, frames;
            int ticks_per_second;
            double fps_limiter;
            double delta;
            double ticks_to_do;
            double last_frame;
            double second_timer;
        } time;
    };

}