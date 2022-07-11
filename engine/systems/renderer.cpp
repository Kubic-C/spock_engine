#include "renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    renderer2D_tt::renderer2D_tt() {}

    void renderer2D_tt::init(scene_tt& scene, void* data) {
        int width, height;

        window = (sfk::window_tt*)data;

        /* OpenGL Init*/
        {
            window->make_context_current();
            if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                sfk::logger.add_log(sfk::LOG_TYPE_ERROR, "could not load OpenGL with GLAD");
                abort();
            }
        }

        /* renderer systems */
        {
            quad_renderer.init(*window, scene.world, &scene.render_scene->quad_batch);
            renderer_manager.push_system(&quad_renderer);
        }
    }

    void renderer2D_tt::set_window_state() {
        int width, height;

        window->get_size(&width, &height);        
        glViewport(0, 0, width, height);
    }

    void renderer2D_tt::update(scene_tt& scene, float deltatime) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(auto sys : renderer_manager.systems) {
            sys->render(scene);
        }

        window->swap_buffers();
    }   
    
    void renderer2D_tt::resize(void* void_self, int width, int height) {
        renderer2D_tt* self = (renderer2D_tt*)void_self;

        for(auto sys : self->renderer_manager.systems) {
            self->set_window_state();
            sys->resize(width, height);
        }
    }

    void renderer2D_tt::free() {
        renderer_manager.free();
    }
}