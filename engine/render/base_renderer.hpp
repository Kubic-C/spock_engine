#pragma once

#include "meshes.hpp"
#include "systems.hpp"

namespace spk {
    class render_system_t;

    struct framebuffer_t {
        static_list<id_t, 8> attachments;
        uint32_t              framebuffer;
        uint32_t              clear_bits;
        glm::vec4             clear_color;
    };

    struct render_ctx_t {
        void init();
        void free();

        vertex_layout_t               vertex_layout;
        vertex_array_t                vertex_array;
        program_t                     program;
    };

    class base_renderer_t : public system_t {
    public:
        virtual void draw() {}

        static_list<flecs::entity, 8> systems;  
    protected:
    };

    class base_framebuffer_renderer_t : public system_t {
    public:
        virtual void draw(render_system_t* rs, id_t current) {}
    };
}