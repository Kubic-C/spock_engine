#pragma once
#include "../systems.hpp"
#include "bx/math.h"

namespace spk {
    class renderer2D_tt : public system_tt {
    public:
        struct init_data {
            sfk::window* window;
            sfk::debug_logger* logger;
        };

        renderer2D_tt();

        void init(void* data);
        void update(flecs::world& world);
        void tick(flecs::world& world) {}
        void free();

        void render();

        void set_window_state();

        struct vertex {
            float x, y, z;
            uint32_t agbr;
        };
    private:
        sfk::window* window;
        bgfx::VertexBufferHandle triangle_buffer; 
        bgfx::IndexBufferHandle index_buffer;

        struct {

            bgfx::VertexLayout layout;
            bgfx::ProgramHandle program;
        } triangle;
    };
}