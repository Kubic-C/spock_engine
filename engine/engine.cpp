#include "engine.hpp"

namespace spk {
    engine::engine() {
        sfk::zero<engine::time_tt>(&this->time);
    }

    void print_name2() {
        sfk::print_name();
    }

    void engine::init(int w, int h, std::string title) {
        set_tick_speed(60);

        framework.init();
        window.init(w, h, title);

        renderer2D_tt::init_data renderer_init;
        renderer_init.window = &window;
        renderer_init.logger = &framework.get_logger();
        renderer.init(&renderer_init);

        system_manager.push_system(&renderer);
    }

    void engine::free() {
        system_manager.free();

        window.free();
        framework.free();
    }

    void engine::set_tick_speed(int tick_speed) {
        time.ticks_per_second = tick_speed;
        time.fps_limiter = 1.0f / tick_speed;
    }

    void engine::loop() {
        while(!window.closed()) {
            float current_frame = framework.get_time();
            time.delta = (current_frame - time.last_frame);
            time.ticks_to_do += time.delta / time.fps_limiter;
            time.last_frame = current_frame;

            while(time.ticks_to_do >= 1.0f) {
                time.ticks_to_do--;

                system_manager.tick();
                time.ticks++;
            }

            {
                update();
                system_manager.update();
                system_manager.msg_update();
                time.frames++;
            }
        
            if(framework.get_time() - time.second_timer > 1.0) {
                time.second_timer++;
                printf("FPS: %4u | UPS: %3u | DELTA: %1.8f\n", time.frames, time.ticks, time.delta);
                time.frames = 0, time.ticks = 0;
                std::cout.flush();
            }
        }
    }

    void engine::update() {
        glfwPollEvents();
    }
}