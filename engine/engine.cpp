#include "engine.hpp"

namespace spk {
    engine_tt::engine_tt() {
        sfk::zero<engine_tt::time_tt>(&this->time);
    }

    void print_name2() {
        sfk::print_name();
    }

    void engine_tt::init(int w, int h, std::string title) {
        set_tick_speed(60);

        framework.init();
        window.gl_version(3, 3);
        window.init(w, h, title);

        scene = new scene_tt;
        scene->window = &window;
        scene->world.component<transform_tt>();
        scene->render_scene = new render_scene_tt;
        scene->physics_scene = new physics_scene_tt;
        
        {
            renderer.init(*scene, nullptr);
            renderer.name = "renderer2D";
            renderer.sorting_order = DEFAULT_SORTING_ORDER;

            window.resize_callback.fp_callback = renderer.resize;
            window.resize_callback.data = &renderer;
        }

        {
            physics.init(*scene, nullptr);
            physics.name    = "physics2D";
            physics.sorting_order = DEFAULT_SORTING_ORDER;
        }

        {
            input.init(*scene, &window);
            input.name = "input";
            input.sorting_order = DEFAULT_SORTING_ORDER;
        }

        system_manager.push_system(&renderer);
        system_manager.push_system(&physics);
        system_manager.push_system(&input);
    }

    void engine_tt::free() {
        system_manager.free_user_systems();
        
        // we want the user to have access to entities when
        // it is time to free. setting world.m_owned to false prevents
        // the wrapper from deconstructing the world AGAIN
        ecs_fini(scene->world.get_world());
        scene->world.m_owned = false; 
        delete scene->physics_scene;
        delete scene->render_scene;
        delete scene;

        system_manager.free();

        window.free();
        framework.free();
    }

    void engine_tt::push_system(system_tt* system) {
        system_manager.push_system(system);
        system->init(*scene, (void*)this);
    }

    void engine_tt::set_tick_speed(int tick_speed) {
        time.ticks_per_second = tick_speed;
        time.fps_limiter = 1.0f / tick_speed;
    }

    void engine_tt::loop() {
        while(!window.closed()) {
            float current_frame = framework.get_time();
            time.delta = (current_frame - time.last_frame);
            time.ticks_to_do += time.delta / time.fps_limiter;
            time.last_frame = current_frame;

            while(time.ticks_to_do >= 1.0f) {
                time.ticks_to_do--;

                system_manager.tick(*scene, time.delta);
                time.ticks++;
            }

            {
                update();
                system_manager.update(*scene, time.delta);
                system_manager.msg_update(*scene, time.delta);
                time.frames++;
            }
        
            if(framework.get_time() - time.second_timer > 1.0) {
                time.second_timer++;
                printf("FPS: %4u | UPS: %3u | DELTA: %1.8f | RUNTIME: %5.3f\n", time.frames, time.ticks, time.delta, scene->world.time());
                time.frames = 0, time.ticks = 0;
                std::cout.flush();
            }
        }
    }

    void engine_tt::update() {
        glfwPollEvents();
        scene->world.progress(time.delta);
    }
}