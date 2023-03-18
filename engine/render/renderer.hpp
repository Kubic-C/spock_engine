#pragma once

#include "body_renderer.hpp"
#include "sprite_renderer.hpp"

namespace spk {
    constexpr size_t _render_default_buffer_size = 4096 * sizeof(float);
    constexpr size_t _render_max_quad_indexes    = 25000;

    enum shader_type_e {
        SHADER_TYPE_BASIC = 0,
        SHADER_TYPE_SPRITE = 1,
        SHADER_TYPE_END = 0xFF // 255
    };

    enum renderer_type_e {
        RENDERER_TYPE_BODY = 0,
        RENDERER_TYPE_SPRITE = 1,
        RENDERER_TYPE_END = 0xFF // 255
    };

    struct render_context_t {
        render_context_t();

        void frame_resize(int width, int height);
        void frame_begin();
        void frame_render();
        void frame_end();

        // shader programs
        hashmap_t<shader_type_e, program_t> shaders;

        // renderers
        hashmap_t<renderer_type_e, base_renderer_t*> renderers;
        stack_allocator_t renderer_stack; // used ONLY for renderers

        // cameras
        glm::mat4 world_camera; 
        glm::mat4 ui_camera;

        // clear coloring
        glm::vec3 clear_color = {0.1f, 0.1f, 0.1f};

        // buffer: allocated with an initial size of _render_default_buffer_size
        // array: vertex array with no bound attributes or buffers, must be set before use
        struct {
            vertex_buffer_t       buffer;
            vertex_array_t        array;
            static_index_buffer_t quad_indexes;
        } common;
    };

    render_context_t& render_context();
    
    // sets the passed context as the current rendering context (different from OpenGL context)
    void render_context_make_current(render_context_t& render_context);
}