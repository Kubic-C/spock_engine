/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "common.hpp"
#include "render_pass.hpp"
#include "render_pass.hpp"
#include "body_renderer.hpp"
#include "sprite_renderer.hpp"
#include "canvas_renderer.hpp"
#include "tilemap_renderer.hpp"

namespace spk {
    constexpr size_t _render_default_buffer_size = 8000 * sizeof(float);
    constexpr size_t _render_max_quad_indexes    = 6000;

    struct render_context_t {
        render_context_t();

        void frame_resize(int width, int height);
        void frame_begin();
        void frame_render();
        void frame_end();

        render_info_t render;

        // framebuffers
        std::list<attachment_t, ptm::memory_pool_t<attachment_t>> attachments;
        hashmap_t<framebuffer_type_e, framebuffer_t>         framebuffers;

        // shader programs
        hashmap_t<shader_type_e, program_t> shaders;

        // renderers
        hashmap_t<renderer_type_e, ptm::ptr_t<base_renderer_t>> renderers;
        ptm::stack_allocator_t renderer_stack; // used ONLY for renderers

        // cameras
        glm::mat4 world_camera; 
        glm::mat4 ui_camera;

        // buffer: allocated with an initial size of _render_default_buffer_size
        // array: vertex array with no bound attributes or buffers, must be set before use
        struct {
            vertex_buffer_t       buffer;
            vertex_array_t        array;
            static_index_buffer_t quad_indexes;
        } common;

        // used within framebuffer_ops, all the resources they use
        struct {
            glm::mat4 one_to_one_vp;

            struct {
                vertex_array_t array;
                vertex_buffer_t buffer;
            } overlay;

        } framebuffer_ops;
    
    private:
        void _framebuffers_init();
        void _framebuffers_free();

        void _framebuffer_ops_init();
        void _framebuffer_ops_free();
    };

    render_context_t& render_context();
    
    // sets the passed context as the current rendering context (different from OpenGL context)
    void render_context_make_current(render_context_t& render_context);
}