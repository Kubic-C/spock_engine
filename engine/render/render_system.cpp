#include "render_system.hpp"
#include "core/internal.hpp"

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
 
    render_system_t:: render_system_t() {
        spk_trace();

#ifndef NDEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(opengl_debug_callback, nullptr);
#endif

        // setup the default framebuffer and render pass
        fb_default_init();

        rp_init();
        rp_set_fb(0, 0);

        glEnable(GL_DEPTH_TEST);

        // setup utilities
        quad_index_buffer.init(GL_ELEMENT_ARRAY_BUFFER);
        quad_index_buffer.generate_quad_indexes(100000);

        copy_buffer.init(GL_ARRAY_BUFFER);
    }

    render_system_t::~render_system_t() {
        spk_trace();
        
        copy_buffer.free();
        quad_index_buffer.free();
    }

    id_t render_system_t::atch_init() {
        spk_trace();

        attachment_t atch;
        atch.attachment      = UINT32_MAX;
        atch.pixel_format    = UINT32_MAX;
        atch.pixel_type      = UINT32_MAX;
        atch.internal_format = UINT32_MAX;

        glGenTextures(1, &atch.texture);        

        attachments.push_back(atch);

        return attachments.get_size() - 1;
    }

    void render_system_t::resize(uint32_t width, uint32_t height) {
        spk_trace();

        for(attachment_t& atch : attachments) {
            glBindTexture(GL_TEXTURE_2D, atch.texture);
            glTexImage2D(GL_TEXTURE_2D, 0, atch.internal_format, width, height, 0, atch.pixel_format, atch.pixel_type, nullptr);
        }

        glViewport(0, 0, width, height);
    }
    void render_system_t::atch_set(id_t atch_id, uint32_t type, uint32_t internal_format, uint32_t pixel_format, uint32_t pixel_type) {
        spk_trace();

        attachment_t& atch = attachments[atch_id];
        
        atch.attachment      = type;
        atch.internal_format = internal_format;
        atch.pixel_format    = pixel_format;
        atch.pixel_type      = pixel_type;

        glBindTexture(GL_TEXTURE_2D, atch.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, 100, 100, 0, pixel_format, pixel_type, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    attachment_t& render_system_t::atch_get(id_t atch_id) {
        spk_trace();

        return attachments[atch_id];
    }

    void render_system_t::_atch_free(id_t atch_id) {
        spk_trace();

        framebuffers[atch_id].attachments.clear();
        glDeleteTextures(1, &attachments[atch_id].texture);
    }
    
    void render_system_t::atch_remove(id_t atch_id) {
        spk_trace();

        attachments.erase(atch_id);
    }

    void render_system_t::fb_default_init() {
        spk_trace();

        framebuffer_t fb;
        fb.clear_bits  = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
        fb.clear_color = {0.3f, 0.3f, 0.3f, 1.0f};
        fb.framebuffer = 0;        

        framebuffers.push_back(fb);
    }

    id_t render_system_t::fb_init() {
        spk_trace();
        
        framebuffer_t fb;
        fb.clear_bits  = GL_COLOR_BUFFER_BIT;
        fb.clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
        
        glGenFramebuffers(1, &fb.framebuffer);

        framebuffers.push_back(fb);
    
        return framebuffers.get_size() - 1;
    }

    framebuffer_t& render_system_t::fb_get(id_t key) {
        return framebuffers[key];
    }

    void render_system_t::fb_set_clear_bits(id_t fb_id, uint32_t bits) {
        spk_trace();

        framebuffers[fb_id].clear_bits = bits;
    }

    void render_system_t::fb_set_clear_color(id_t fb_id, float r, float g, float b, float a) {
        spk_trace();

        framebuffers[fb_id].clear_color = {r, g, b, a};
    }

    void render_system_t::fb_attach(id_t fb_id, id_t atch_id) {
        spk_trace();

        framebuffers[fb_id].attachments.push_back(atch_id);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[fb_id].framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[atch_id].attachment, GL_TEXTURE_2D, attachments[atch_id].texture, 0);
    }

    void render_system_t::_fb_free(id_t fb_id) {
        spk_trace();

        framebuffers[fb_id].attachments.clear();
        glDeleteFramebuffers(1, &framebuffers[fb_id].framebuffer);
    }

    void render_system_t::fb_remove(id_t fb_id) {
        spk_trace();

        framebuffers.erase(fb_id);
    }

    id_t render_system_t::rp_init() {
        spk_trace();

        render_pass_t rp;
        rp.framebuffer = 0;
        rp.fb_renderer = nullptr;

        render_passes.push_back(rp);
    
        return render_passes.get_size() - 1;
    }

    render_pass_t& render_system_t::rp_get(id_t rp_id) {
        return render_passes[rp_id];
    }

    void  render_system_t::rp_set_fb(id_t rp_id, id_t fb_id) {
        render_passes[rp_id].framebuffer = fb_id;
    }

    void  render_system_t::rp_add_renderer(id_t rp_id, base_renderer_t* renderer) {
        spk_trace();

        render_passes[rp_id].renderer.push_back(renderer);
    }
    
    void render_system_t::rp_set_fb_renderer(id_t rp_id, base_framebuffer_renderer_t* renderer) {
        render_passes[rp_id].fb_renderer = renderer;
    }

    void  render_system_t::_rp_free(id_t rp_id) {
        spk_trace();

        render_passes[rp_id].renderer.clear();
    }

    void render_system_t::rp_remove(id_t rp_id) {
        spk_trace();

        render_passes.erase(rp_id);
    }

    void render_system_t::begin_frame() {

    }

    void render_system_t::draw_frame() {
        spk_trace();
        
        for(auto& rp : render_passes) {
            spk_assert(rp.framebuffer < framebuffers.get_size());
            framebuffer_t* fb = &framebuffers[rp.framebuffer];

            glBindFramebuffer(GL_FRAMEBUFFER, fb->framebuffer);
            glClearColor(fb->clear_color.r, fb->clear_color.g, fb->clear_color.b, fb->clear_color.a);
            glClear(fb->clear_bits); 

            for(auto renderer : rp.renderer) {
                for(auto& system : renderer->systems) {
                    flecs::system(system.world().m_world, system.id()).run(internal->statistics.delta_time);
                }

                renderer->draw();
            }

            if(rp.fb_renderer)
                rp.fb_renderer->draw(this, rp.framebuffer);
        }
    }

    void render_system_t::end_frame(SDL_Window* window) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[0].framebuffer);

        SDL_GL_SwapWindow(window);
    }
}