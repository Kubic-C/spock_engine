#include "engine.hpp"

namespace spk {
    engine_t::engine_t() {
    }

    void print_name2() {
        sfk::print_name();
    }

    void engine_t::init(int w, int h, std::string title) {
        set_tick_speed(60);

        framework.init();
        window.gl_version(4, 3);
        window.init(w, h, title);

        scene = new scene_t;
        scene->engine = this;
        scene->window = &window;
        scene->render_scene = new render_scene_t;
        scene->physics_scene = new physics_scene_t;
        
        DEBUG_VALUE(bool, ret =) resource_manager.init();
        sfk_assert(ret);
        
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
            ui.init(*scene, nullptr);
            ui.name = "user interface";
            ui.sorting_order = DEFAULT_SORTING_ORDER;

            window.mouse_callback.data = &ui;
            window.mouse_callback.fp_callback = ui.mouse_button_callback;
        }

        system_manager.push_system(&renderer);
        system_manager.push_system(&physics);
        system_manager.push_system(&ui);
    }

    void engine_t::free() {
        system_manager.free_user_systems(*scene);
    
        // we want the user to have access to entities when
        // it is time to free. setting world.m_owned to false prevents
        // the wrapper from deconstructing the world AGAIN
        ecs_fini(scene->world.get_world());
        scene->world.m_owned = false; 
        
        system_manager.free(*scene);

        delete scene->physics_scene;
        delete scene->render_scene;
        delete scene;

        resource_manager.free();
        window.free();
        framework.free();
    }

    void engine_t::push_system(system_t* system) {
        system_manager.push_system(system);
        system->init(*scene, (void*)this);
    }

    void engine_t::push_render_system(render_system_t* system) {
        renderer.renderer_manager.push_system(system);
    }

    void engine_t::set_tick_speed(int tick_speed) {
        time.ticks_per_second = tick_speed;
        time.fps_limiter = 1.0f / tick_speed;
    }

    void engine_t::loop() {
        scene->world.system("update").interval(0.0).ctx(this).no_staging(true).iter(update_sys);
        scene->world.system("tick").interval(1.0f/60.0f).ctx(this).no_staging(true).iter(tick);
        scene->world.system("print_stats").interval(1.0).ctx(this).no_staging(true).iter(print_stats);

        while(!window.closed() && scene->world.progress(0)) {}
    }

    void engine_t::update() {
        glfwPollEvents();
    }

    float engine_t::get_elapsed_time() {
        return time.last_frame - sfk::time.get_time();
    }

    void engine_t::print_stats(flecs::iter& iter) {
        spk::engine_t* engine = (spk::engine_t*)iter.ctx();

        printf("FPS: %4u | UPS: %3u | DELTA: %1.8f | RUNTIME: %5f\n", engine->time.frames, engine->time.ticks, engine->time.delta, engine->scene->world.time());
        engine->time.frames = 0, engine->time.ticks = 0;
    }

    void engine_t::update_sys(flecs::iter& iter) {
        spk::engine_t* engine = (spk::engine_t*)iter.ctx();

        engine->update();
        engine->system_manager.update(*engine->scene,  engine->time.delta);
        engine->time.frames++;
    }

    void engine_t::tick(flecs::iter& iter) {
        spk::engine_t* engine = (spk::engine_t*)iter.ctx();

        if(engine->time.exit != INT_MAX) {
             engine->time.exit--;

            if(engine->time.exit <= 0)
                engine->scene->window->close();
        }

        engine->system_manager.tick(*engine->scene,  engine->time.delta);
        engine->time.ticks++;
    }
}