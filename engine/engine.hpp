#pragma once
#include "framework.hpp"
#include "systems.hpp"
#include "systems/renderer.hpp"

namespace spk {
    void print_name2();

    class engine {
    public:
        engine();

        void init(int w, int h, std::string title);
        void free();

        void set_tick_speed(int tick_speed);

        void update();
        void loop();

    private:
        system_manager_tt system_manager;
        sfk::sfk framework;
        renderer2D_tt renderer;
        sfk::window window;
        
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