#pragma once

#include "../systems.hpp"
#include "../utility/physics_.hpp"

namespace spk {
    struct collider_render_t {
        glm::vec3 color;

        collider_render_t() {
            sfk::zero(this);

            color = (glm::vec3){1.0f, 0.0f, 0.0f};
        }
    };

    // quick and dirty rendering
    class collider_renderer_t : public render_system_t {
    public:

        void init(scene_t& scene);
        void render(scene_t& scene);
        void resize(int width, int height);
        void free(scene_t& scene);

        void render_polygon(glm::mat4& vp, collider_render_t* primitive, b2Body* body, b2PolygonShape* polygon);
        void render_circle(glm::mat4& vp, collider_render_t* primitve, b2Body* body, b2CircleShape* circle);

    private:    
        struct vertex_t {
            float x, y; // position
            float r, g, b; // color
        };

        sfk::vertex_buffer_t vertex_buffer;
        sfk::static_index_buffer_t index_buffer;
        sfk::vertex_layout_t vertex_layout;
        sfk::vertex_array_t  vertex_array;
        sfk::program_t program;

        flecs::query<spk::comp_b2Body, spk::collider_render_t> q;

        std::vector<vertex_t> mesh;
        uint32_t vertices; // of vertex vvv
    };
}