#pragma once
#include "framework.hpp"
#include "systems/renderer.hpp"
#include "systems/physics.hpp" 
#include "systems/input.hpp"

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

        void set_tick_speed(int tick_speed);
        void update();
        void loop();

    private:
        sfk::sfk framework;
        renderer2D_tt renderer;
        physics2D_tt physics;
        input_tt input;

        system_manager_tt system_manager;

        struct time_tt {
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