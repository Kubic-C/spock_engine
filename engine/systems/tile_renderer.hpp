#pragma once

#include "../systems.hpp"
#include "../utility/tile_body.hpp"
#include "collider_renderer.hpp"

namespace spk {
    struct tile_render_scene_t {
    };

    struct tile_render_t {};

    class tile_renderer_t : public render_system_t {
    public:
        void init(scene_t& scene);
        void render(scene_t& scene);
        void resize(int width, int height);
        void free(scene_t& scene);

        void dirty_rendering(glm::mat4& vp, collider_render_t* primitive, b2Body* body, b2PolygonShape* polygon);

    private:
        struct vertex_t {
            float x, y;
            float u, v;
        };

        sfk::vertex_buffer_t vertex_buffer;
        sfk::static_index_buffer_t index_buffer;
        sfk::vertex_layout_t vertex_layout;
        sfk::vertex_array_t  vertex_array;
        sfk::program_t program;

        std::vector<vertex_t> mesh;
        flecs::query<tile_render_t, tile_body_t> q;        
    };
}