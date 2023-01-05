#pragma once

#include "box2D.hpp"
#include "data/atlas.hpp"

namespace spk {
    struct comp_sprite_atlasd_t {
        void init() {}
        void free() {}

        uint32_t  atlas_id = 0; // the sprite atlas to get sprite data from
        uint32_t  tax      = 0, 
                  tay      = 0; // texture coord offsets in the sprite atlas
        glm::vec2 size     = {0.5f, 0.5f}; // the size of the sprite to render
        float     z        = 0.0f; // z coordinate, positive z is towards the camera

        b2Fixture* add_fixture(b2Body* body, float density, float friction, float restitution, int group_index = 0);
    };

    // comp_sprite_arrayd_t is stored within a Texture Array rather then a Texture atlas.
    struct comp_sprite_arrayd_t {
        void init() {}
        void free() {}
      
        uint32_t  array_id = 0; // the sprite texture array to get sprite data from
        uint32_t  index    = 0; // the level within in the the texture array
        glm::vec2 size     = {0.5f, 0.5f};
        float     z        = 0.0f; // z coordinate, positive z is towards the camera

        b2Fixture* add_fixture(b2Body* body, float density, float friction, float restitution, int group_index = 0);
    };

    void sprite_comp_init(flecs::world& world);
}