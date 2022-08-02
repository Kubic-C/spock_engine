#pragma once

#include "../systems.hpp"
#include "../utility/batching.hpp"

namespace spk {
    struct quad_scene_tt {
    };

    struct quad_render_tt {
        quad_render_tt() {
            sfk::zero(this);
        }
    };

    class quad_renderer_tt : public render_system_tt {
    public:

        void init(sfk::window_tt& window, flecs::world& world, void* scene);
        void render(scene_tt& scene);
        void resize(int width, int height);
        void free();

    private:    
        struct vertex {
            float x, y; // position
            float u, v; // tex_coord
        };

        sfk::vertex_buffer_tt vertex_buffer;
        sfk::vertex_layout_tt vertex_layout;
        sfk::vertex_array_tt  vertex_array;
        uint32_t texture;
        uint32_t program;

        glm::mat4 vp;
        glm::mat4 proj;
        glm::mat4 view;

        std::vector<vertex> mesh;
        uint32_t vertices; // of vertex vvv
    };
}