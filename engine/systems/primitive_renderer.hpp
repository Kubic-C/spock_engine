#pragma once

#include "../systems.hpp"

namespace spk {
    struct quad_scene_tt {
    };

    struct primitive_render_tt {
        glm::vec3 color;

        primitive_render_tt() {
            sfk::zero(this);

            color = (glm::vec3){1.0f, 0.0f, 0.0f};
        }
    };

    class primitive_renderer_tt : public render_system_tt {
    public:

        void init(scene_tt& scene);
        void render(scene_tt& scene);
        void resize(int width, int height);
        void free();

    private:    
        struct vertex_tt {
            float x, y; // position
            float r, g, b; // color
        };

        sfk::vertex_buffer_tt vertex_buffer;
        sfk::static_index_buffer_tt index_buffer;
        sfk::vertex_layout_tt vertex_layout;
        sfk::vertex_array_tt  vertex_array;
        uint32_t texture;
        sfk::program_tt program;

        glm::mat4 vp;
        glm::mat4 proj;
        glm::mat4 view;

        std::vector<vertex_tt> mesh;
        uint32_t vertices; // of vertex vvv
    };
}