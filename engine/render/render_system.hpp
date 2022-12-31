#pragma once

#include "base_renderer.hpp"

namespace spk {
    struct attachment_t {
        uint32_t texture;
        uint32_t internal_format;
        uint32_t data_format;
        uint32_t attachment;
    };

    struct render_pass_t {
        static const size_t max_renderers      = 8;

        key_t                                        framebuffer;
        static_list<base_renderer_t*, max_renderers> renderer; 
        base_framebuffer_renderer_t*                 fb_renderer;
    };

    class render_system_t : public system_t {
    public:
        static const size_t max_framebuffers   = 8;
        static const size_t max_attachments    = 8;
        static const size_t max_render_passes  = 8; 

        static_index_buffer_t quad_index_buffer;
        vertex_buffer_t       copy_buffer;

        void init() override;
        void free() override;

        void resize(uint32_t width, uint32_t height);

        key_t         atch_init();
        attachment_t& atch_get(key_t atch_id);
        void          atch_set(key_t atch_id, uint32_t type, uint32_t internal_format, uint32_t data_format);
        void          _atch_free(key_t atch_id);
        void          atch_remove(key_t atch_id);

    
        void           fb_default_init();
        key_t          fb_init();
        framebuffer_t& fb_get(key_t fb_id);
        void           fb_set_clear_bits(key_t fb_id, uint32_t bits);
        void           fb_set_clear_color(key_t fb_id, float r, float g, float b, float a);
        void           fb_attach(key_t fb_id, key_t atch_id);
        void           _fb_free(key_t fb_id);
        void           fb_remove(key_t fb_id);

        key_t          rp_init();
        render_pass_t& rp_get(key_t rp_id);
        void           rp_set_fb(key_t rp_id, key_t fb_id);
        void           rp_add_renderer(key_t rp_id, base_renderer_t* renderer);
        void           _rp_free(key_t rp_id);
        void           rp_remove(key_t fb_id);

        void begin_frame();
        void draw_frame();
        void end_frame(SDL_Window* window);

    private:
        static_list<attachment_t, max_attachments> attachments;
        static_list<framebuffer_t, max_framebuffers> framebuffers;
        static_list<render_pass_t, max_render_passes> render_passes;
    };
}
