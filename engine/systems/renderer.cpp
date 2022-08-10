#include "renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    renderer2D_tt::renderer2D_tt() {}

    void renderer2D_tt::init(scene_tt& scene, void* data) {
        int width, height;

        /* OpenGL Init*/
        {
            scene.window->make_context_current();
            if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                sfk::logger.add_log(sfk::LOG_TYPE_ERROR, "could not load OpenGL with GLAD");
                abort();
            }            
        }

        /* renderer systems */
        {
            primitive_renderer.init(scene);
            ui_renderer.init(scene);

            renderer_manager.push_system(&primitive_renderer);
            renderer_manager.push_system(&ui_renderer);
        }

        scene.window->get_size(&width, &height);
        resize(scene.window, (void*)this, width, height);
    }

    void renderer2D_tt::update(scene_tt& scene, float deltatime) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(auto sys : renderer_manager.systems) {
            sys->render(scene);
        }

        scene.window->swap_buffers();

    }   
    
    void renderer2D_tt::resize(sfk::window_tt* window, void* void_self, int width, int height) {
        renderer2D_tt* self = (renderer2D_tt*)void_self;

        for(auto sys : self->renderer_manager.systems) {
            glViewport(0, 0, width, height);
            sys->resize(width, height);
        }
    }

    void renderer2D_tt::free(scene_tt& scene) {
        renderer_manager.free();
    }
}