#include "renderer.hpp"
#include "core/internal.hpp"
#include "default_shaders.hpp"

namespace spk {
#ifndef CALLBACK
    #define CALLBACK 
#endif
  
    void CALLBACK opengl_debug_callback(GLenum source,
            GLenum type,
            GLenum severity,
            GLuint id,                  
            GLsizei length,
            const GLchar *message,
            const void *userParam) {
        spk_trace();

        switch(type) {
        case GL_DEBUG_TYPE_ERROR:
            log.log(spk::LOG_TYPE_ERROR, message);
            break;

        default:
            log.flags &= ~spk::LOG_FLAGS_ENABLE_STD_PIPE;
            log.log(spk::LOG_TYPE_INFO, message); 
            log.flags |= spk::LOG_FLAGS_ENABLE_STD_PIPE;
        }
    }

    void add_opengl_debug() {
        #ifndef NDEBUG
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(opengl_debug_callback, nullptr);
        #endif
    }
}

namespace spk {
    render_context_t::render_context_t() {
        add_opengl_debug();

        common.buffer.init(GL_ARRAY_BUFFER);
        common.buffer.buffer_data(_render_default_buffer_size, nullptr, GL_DYNAMIC_DRAW);
        common.array.init();

        common.quad_indexes.init(GL_ELEMENT_ARRAY_BUFFER);
        common.quad_indexes.generate_quad_indexes(_render_max_quad_indexes);

        glEnable(GL_DEPTH_TEST);

        // default shaders
        shaders[SHADER_TYPE_BASIC].load_shader_str(basic_vs, basic_fs);
        shaders[SHADER_TYPE_SPRITE].load_shader_str(sprite_vs, sprite_fs);
        
        // default renderers
        renderers[RENDERER_TYPE_BODY] = renderer_stack.push<body_renderer_t>(ecs_world());
        renderers[RENDERER_TYPE_SPRITE] = renderer_stack.push<sprite_renderer_t>(ecs_world());
    }

    render_context_t& render_context() {
        spk_assert(internal->scene.render_context != nullptr);
        
        return *internal->scene.render_context;
    }
    void render_context_make_current(render_context_t& render_context) {
        spk_assert(internal->scene.render_context == nullptr, "rendering context can only be set once");

        internal->scene.render_context = &render_context;
    }

    void render_context_t::frame_resize(int width, int height) {
        glViewport(0, 0, width, height);
    }

    void render_context_t::frame_begin() {
        spk_trace();
        
    }

    void render_context_t::frame_render() {
        spk_trace();


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        for(auto& renderer : renderers) {
            for(auto& mesh_system : renderer.second->mesh_systems) {
                mesh_system.run();
            }

            renderer.second->render();
        }
    }

    void render_context_t::frame_end() {
        spk_trace();

        SDL_GL_SwapWindow(window().window);
    }
}