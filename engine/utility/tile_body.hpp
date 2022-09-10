#pragma once

#include "../scene.hpp"

namespace spk {
    constexpr size_t max_tile_types = 255 ;
    constexpr size_t max_tile_width = 10;
    constexpr size_t max_tile_height = 10;

    struct _tile_render_t {
        glm::vec2 tex_coords[4];
    };

    struct _tile_physics_t {
        b2FixtureDef fix_def;
        b2PolygonShape shape_def;
    };

    struct tile_dictionary_t {
        sfk::hashmap_t<_tile_render_t, max_tile_types, 4> render;
        sfk::hashmap_t<_tile_physics_t, max_tile_types, 4> physics;

        uint32_t texture_width;
        uint32_t texture_height;
        sfk::texture2D_t texture;

        void init();
        void free();

        // boiler code go away funcs
        sfk::key_t init_tile(sfk::key_t key);
        void free_tile(sfk::key_t key);
    };

    struct tile_t {
        sfk::key_t type_id;
        b2Fixture* fixture;
    };

    struct tile_body_t {
        b2Body* body;
        tile_dictionary_t* dict;
        std::array<std::array<tile_t, max_tile_height>, max_tile_width> tiles;

        void init(b2World* world, b2BodyType type, tile_dictionary_t* dict);
        void create_tile(size_t x, size_t y, sfk::key_t type);
        void set_tile(size_t x, size_t y, sfk::key_t type);
        void destroy_tile(size_t x, size_t y);
        void free(b2World* world);
    };

    void add_tile_body_components(scene_t& scene);
}