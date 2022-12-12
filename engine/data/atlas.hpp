#pragma once

#include "framework.hpp"

#define SPK_MAX_ATLAS 8

namespace spk {
    struct sprite_atlas_t {
        bool init(uint32_t sprites_per_row, uint32_t sprites_per_column);
        void free();
       
        bool load_from_path(const char* path);
        std::array<glm::vec2, 4> gen_tex_coords(uint32_t tax, uint32_t tay);

        glm::uvec2 size;
        float width_per_sprite;
        float height_per_sprite;
        uint32_t sprites_per_row;
        uint32_t sprites_per_column;
        sfk::texture2D_t texture;
    };

    class atlas_manager_t {
    public:
        bool am_init();
        void am_free();
        bool atlas_is_in_use(uint32_t index);
        void atlas_in_use(uint32_t index);
        sprite_atlas_t* get_atlas(uint32_t index);

        void atlas_init(uint32_t index, uint32_t sprites_per_row, uint32_t sprites_per_column) {
            sprite_atlas_t* atlas = &atlases[index];

            atlas->init(sprites_per_row, sprites_per_column);
            atlas_in_use(index);
        }

        bool atlas_load_from_path(uint32_t index, const char* path) {
            sprite_atlas_t* atlas = get_atlas(index);

            return atlas->load_from_path(path);
        }
    private:
        std::bitset<SPK_MAX_ATLAS> in_use;
        std::array<sprite_atlas_t, SPK_MAX_ATLAS> atlases;
    };
}