#pragma once

#include "components.hpp"
#include "core/sprite_array.hpp"
#include "core/sprite_atlas.hpp"

namespace spk {
    // Not used with in the engine
    struct sprite_atlasd_t : component_t {
        uint32_t  atlas_id = 0; // the sprite atlas to get sprite data from
        uint32_t  tax      = 0, 
                  tay      = 0; // texture coord offsets in the sprite atlas
        glm::vec2 size     = {0.5f, 0.5f}; // the size of the sprite to render
        float     z        = 0.0f; // z coordinate, positive z is towards the camera
    };

    // sprite_arrayd image info is stored within a Texture Array rather then a Texture atlas.
    struct sprite_arrayd_t : component_t {
        uint32_t  array_id = 0; // the sprite texture array to get sprite data from
        uint32_t  index    = 0; // the level within in the the texture array
        glm::vec2 size     = {0.5f, 0.5f};
        glm::vec2 offset   = {0.0f, 0.0f};
        float     z        = 0.0f; // z coordinate, positive z is towards the camera
    };

    enum sprite_type_e {
        SPRITE_TYPE_ATLAS,
        SPRITE_TYPE_ARRAY
    };

    struct comp_sprite_t : sprite_arrayd_t {
        void init(flecs::entity entity) {}
        void free(flecs::entity entity) {}
    };

    void sprite_comp_init(flecs::world& world);
}