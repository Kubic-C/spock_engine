#pragma once

#include "base_renderer.hpp"

namespace spk {
    struct attachment_t {
        uint32_t texture;
        uint32_t internal_format;
        uint32_t pixel_format;
        uint32_t pixel_type;
        uint32_t attachment;
    };

    struct render_pass_t {
        static const size_t max_renderers      = 8;

        id_t                                        framebuffer;
        static_list<base_renderer_t*, max_renderers> renderer; 
        base_framebuffer_renderer_t*                 fb_renderer;
    };

    class render_system_t {
    public:
        static const size_t max_framebuffers   = 8;
        static const size_t max_attachments    = 8;
        static const size_t max_render_passes  = 8; 

        static_index_buffer_t quad_index_buffer;
        vertex_buffer_t       copy_buffer;

        render_system_t();
        ~render_system_t();

        void resize(uint32_t width, uint32_t height);

        id_t          atch_init();
        attachment_t& atch_get(id_t atch_id);
        void          atch_set(id_t atch_id, uint32_t type, uint32_t internal_format, uint32_t pixel_format, uint32_t pixel_type);
        void          _atch_free(id_t atch_id);
        void          atch_remove(id_t atch_id);

    
        void           fb_default_init();
        id_t           fb_init();
        framebuffer_t& fb_get(id_t fb_id);
        void           fb_set_clear_bits(id_t fb_id, uint32_t bits);
        void           fb_set_clear_color(id_t fb_id, float r, float g, float b, float a);
        void           fb_attach(id_t fb_id, id_t atch_id);
        void           _fb_free(id_t fb_id);
        void           fb_remove(id_t fb_id);

        id_t           rp_init();
        render_pass_t& rp_get(id_t rp_id);
        void           rp_set_fb(id_t rp_id, id_t fb_id);
        void           rp_add_renderer(id_t rp_id, base_renderer_t* renderer);
        void           rp_set_fb_renderer(id_t rp_id, base_framebuffer_renderer_t* renderer);
        void           _rp_free(id_t rp_id);
        void           rp_remove(id_t fb_id);

        void begin_frame();
        void draw_frame();
        void end_frame(SDL_Window* window);

    private:
        static_list<attachment_t, max_attachments> attachments;
        static_list<framebuffer_t, max_framebuffers> framebuffers;
        static_list<render_pass_t, max_render_passes> render_passes;
    };
}
