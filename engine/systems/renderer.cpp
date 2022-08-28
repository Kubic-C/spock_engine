#include "renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    renderer2D_t::renderer2D_t() {}

    void renderer2D_t::init(scene_t& scene, void* data) {
        int width, height;

        /* OpenGL Init*/
        {
            scene.window->make_context_current();
            if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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

    void renderer2D_t::update(scene_t& scene, float deltatime) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(auto sys : renderer_manager.systems) {
            sys->render(scene);
        }

        scene.window->swap_buffers();

    }   
    
    void renderer2D_t::resize(sfk::window_t* window, void* void_self, int width, int height) {
        renderer2D_t* self = (renderer2D_t*)void_self;

        for(auto sys : self->renderer_manager.systems) {
            glViewport(0, 0, width, height);
            sys->resize(width, height);
        }
    }

    void renderer2D_t::free(scene_t& scene) {
        renderer_manager.free();
    }
}