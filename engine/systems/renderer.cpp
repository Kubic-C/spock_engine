#include "renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace spk {
    void opengl_debug_callback(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam) {
        switch(type) {
        case GL_DEBUG_TYPE_ERROR:
            sfk::log.log(sfk::LOG_TYPE_ERROR, message);
            break;

        default:
            sfk::log.flags &= ~sfk::LOG_FLAGS_ENABLE_STD_PIPE;
            sfk::log.log(sfk::LOG_TYPE_INFO, message); 
            sfk::log.flags |= sfk::LOG_FLAGS_ENABLE_STD_PIPE;
        }
    }

    renderer2D_t::renderer2D_t() {}

    void renderer2D_t::init(scene_t& scene, void* data) {
        int width, height;

        /* OpenGL Init*/
        {
            scene.window->make_context_current();
            if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            }            
        }

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(opengl_debug_callback, nullptr);

        /* renderer systems */
        {
            primitive_renderer.init(scene);
            ui_renderer.init(scene);
            tile_renderer.init(scene);

            renderer_manager.push_system(&primitive_renderer);
            renderer_manager.push_system(&ui_renderer);
            renderer_manager.push_system(&tile_renderer);
        }

        this->scene = &scene;
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
        float half_width  = (float)width / 4;
        float half_height = (float)height / 4;
        glm::mat4 view, proj;

        view = glm::identity<glm::mat4>();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        proj = glm::ortho(-(float)half_width / sfk::ppm, (float)half_width / sfk::ppm, -half_height / sfk::ppm, half_height / sfk::ppm);
        self->scene->render_scene->vp   = proj * view;
    
        for(auto sys : self->renderer_manager.systems) {
            glViewport(0, 0, width, height);
            sys->resize(width, height);
        }
    }

    void renderer2D_t::free(scene_t& scene) {
        renderer_manager.free(scene);
    }
}