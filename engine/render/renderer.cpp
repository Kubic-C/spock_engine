/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "renderer.hpp"
#include "core/internal.hpp"
#include "default_shaders.hpp"
#include "glm/gtc/matrix_transform.hpp"

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
            log.log("opengl message ommited; check log");
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

    float entire_display[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
    };
}

namespace spk {

    void render_context_t::_framebuffers_init() {
        framebuffers.emplace(FRAMEBUFFER_TYPE_DEFAULT, true);

        {
            framebuffer_t& framebuffer = framebuffers[FRAMEBUFFER_TYPE_CANVAS];

            // color
            framebuffer.attach(attachments.emplace_back(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0));
            // depth stencil
            framebuffer.attach(attachments.emplace_back(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL_ATTACHMENT));

            if(!framebuffer.check_status()) {
                log.log(LOG_TYPE_ERROR, "failed to create canvas framebuffer");
            }
        }
    }

    void render_context_t::_framebuffers_free() {

    }

    void render_context_t::_framebuffer_ops_init() {
        auto& overlay = framebuffer_ops.overlay;

        framebuffer_ops.one_to_one_vp = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

        overlay.array.init();
        overlay.buffer.init(GL_ARRAY_BUFFER);
        overlay.buffer.buffer_data(sizeof(entire_display), entire_display, GL_STATIC_DRAW);

        overlay.array.bind();
        overlay.buffer.bind();

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)sizeof(glm::vec2));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }

    void render_context_t::_framebuffer_ops_free() {
        auto& overlay = framebuffer_ops.overlay;

        overlay.array.free();
        overlay.buffer.free();
    }

    render_context_t::render_context_t() {
        add_opengl_debug();

        common.buffer.init(GL_ARRAY_BUFFER);
        common.buffer.buffer_data(_render_default_buffer_size, nullptr, GL_DYNAMIC_DRAW);
        common.array.init();

        common.quad_indexes.init(GL_ELEMENT_ARRAY_BUFFER);
        common.quad_indexes.generate_quad_indexes(_render_max_quad_indexes);

        glEnable(GL_DEPTH_TEST);
        
        _framebuffer_ops_init();
        _framebuffers_init();

        // default shaders
        shaders[SHADER_TYPE_BASIC].load_shader_str(basic_vs, basic_fs);
        shaders[SHADER_TYPE_SPRITE].load_shader_str(sprite_vs, sprite_fs);
        shaders[SHADER_TYPE_TEXT].load_shader_str(text_vs, text_fs);
        shaders[SHADER_TYPE_TEXTURE].load_shader_str(texture_vs, texture_fs);
        shaders[SHADER_TYPE_CONTAINER].load_shader_str(container_vs, container_fs);
        shaders[SHADER_TYPE_TILEMAP].load_shader_str(tilemap_vs, sprite_fs);
        
        // default renderers
        renderers[RENDERER_TYPE_TILEMAP]   = renderer_stack.push<tilemap_renderer_t>(ecs_world());
        renderers[RENDERER_TYPE_BODY]      = renderer_stack.push<body_renderer_t>(ecs_world());
        renderers[RENDERER_TYPE_SPRITE]    = renderer_stack.push<sprite_renderer_t>(ecs_world());
        renderers[RENDERER_TYPE_TEXT]      = renderer_stack.push<text_renderer_t>();
        renderers[RENDERER_TYPE_CANVAS]    = renderer_stack.push<canvas_renderer_t>();
        renderers[RENDERER_TYPE_CONTAINER] = renderer_stack.push<container_renderer_t>();

        // setting up the render info
        {
            // world pass will work on the default fb
            render_pass_t& world_pass = render.render_passes.emplace_back();
            world_pass.framebuffer = framebuffers[FRAMEBUFFER_TYPE_DEFAULT];
            world_pass.clear_mask  = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
            world_pass.renderers   = {RENDERER_TYPE_TILEMAP, RENDERER_TYPE_SPRITE, RENDERER_TYPE_BODY};
        }

        {    
            render_pass_t& canvas_pass = render.render_passes.emplace_back(); // renders ontop of the default framebuffer
            canvas_pass.framebuffer     = framebuffers[FRAMEBUFFER_TYPE_CANVAS];
            canvas_pass.dst_framebuffer = framebuffers[FRAMEBUFFER_TYPE_DEFAULT];
            canvas_pass.clear_mask      = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
            canvas_pass.framebuffer_op  = framebuffer_op_overlay;
            canvas_pass.renderers       = {RENDERER_TYPE_CANVAS};
            canvas_pass.clear_color     = {0.0f, 0.0f, 0.0f, 0.0f};
        }
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

        for(auto& attachment : attachments) {
            attachment.resize(width, height);
        }
    }

    void render_context_t::frame_begin() {
        spk_trace();
        
    }

    void render_context_t::frame_render() {
        spk_trace();

        for(auto& render_pass : render.render_passes) {
            glBindFramebuffer(GL_FRAMEBUFFER, render_pass.framebuffer);

            if(render_pass.clear_mask & GL_COLOR_BUFFER_BIT) {
                glClearColor(render_pass.clear_color.r, render_pass.clear_color.g, render_pass.clear_color.b, render_pass.clear_color.a);
            }

            if(render_pass.clear_mask & GL_DEPTH_BUFFER_BIT) {
                glEnable(GL_DEPTH_TEST); 
                glClearDepth(render_pass.clear_depth);
                glDepthFunc(GL_LESS); 
            }

            if(render_pass.clear_mask & GL_STENCIL_BUFFER_BIT) {
                glEnable(GL_STENCIL_TEST);
                glClearStencil(render_pass.clear_stencil);
            }

            glClear(render_pass.clear_mask);

            // call all renderers
            for(auto& renderer_id : render_pass.renderers) {
                ptm::ptr_t renderer = renderers[renderer_id];

                renderer->pre_render();
                renderer->render();
                renderer->post_render();
            }

            if(render_pass.framebuffer_op && render_pass.dst_framebuffer != UINT32_MAX) {
                render_pass.framebuffer_op(render_pass.framebuffer, render_pass.dst_framebuffer);
            }

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST);
        }
    }

    void render_context_t::frame_end() {
        spk_trace();

        SDL_GL_SwapWindow(window().window_get());
    }
}