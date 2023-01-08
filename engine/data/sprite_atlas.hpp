#pragma once

#include "base.hpp"
#include "render/opengl.hpp"

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
        texture2D_t texture;
    };

    class sprite_atlas_manager_t {
    public:
        bool am_init();
        void am_free();
        bool sprite_atlas_is_in_use(uint32_t index);
        void sprite_atlas_in_use(uint32_t index);
        sprite_atlas_t* get_atlas(uint32_t index);

        void sprite_atlas_init(uint32_t index, uint32_t sprites_per_row, uint32_t sprites_per_column) {
            spk_assert(!sprite_atlas_is_in_use(index));

            sprite_atlases[index].init(sprites_per_row, sprites_per_column);
            sprite_atlas_in_use(index);
        }

        bool sprite_atlas_load_from_path(uint32_t index, const char* path) {
            spk_assert(sprite_atlas_is_in_use(index));

            return sprite_atlases[index].load_from_path(path);
        }
    private:
        std::bitset<SPK_MAX_ATLAS> in_use;
        std::array<sprite_atlas_t, SPK_MAX_ATLAS> sprite_atlases;
    };
}