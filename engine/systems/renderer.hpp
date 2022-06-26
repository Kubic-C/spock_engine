#pragma once
#include "tiles.hpp"
#include "bx/math.h"

namespace spk {
    class renderer2D_tt : public system_tt {
    public:
        struct init_data {
            sfk::window* window;
        };

        renderer2D_tt();

        void init(void* data);
        void update(flecs::world& world);
        void tick(flecs::world& world) {}
        void free();

        void set_window_state();

        struct vertex {
            float x, y, z;
            uint32_t agbr;
        };

    private:
        sfk::window* window;

        render_system_manager_tt renderer_manager;
    
        // need a place to access each render system's
        // non-base-class methods
        tile_renderer_tt tile_renderer;
    };
}