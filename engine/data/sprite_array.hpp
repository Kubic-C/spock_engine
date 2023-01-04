#pragma once

#include "render/opengl.hpp"

#define SPK_MAX_SPRITE_ARRAYS 8

namespace spk {
    struct sprite_array_t {
        void init();
        void storage(uint32_t width, uint32_t height, uint32_t sprites);
        bool image(const char* path, uint32_t layer);
        void tex_params();
        void free();

        uint32_t          width;
        uint32_t          height;
        uint32_t          sprites;
        texture_array2D_t texture_array;
    };  

    class sprite_array_manager_t {
    public:
        void sarm_init();
        void sarm_free();

        bool            sprite_array_is_in_use(uint32_t index);
        void            sprite_array_in_use(uint32_t index);

        void            sprite_array_init(uint32_t index);
        void            sprite_array_start(uint32_t index, uint32_t width, uint32_t height, uint32_t sprites);
        bool            sprite_array_load(uint32_t index, const char* path, uint32_t sprite_index);
        void            sprite_array_finish(uint32_t index);
        sprite_array_t* sprite_array_get(uint32_t index);

    private:
        std::bitset<SPK_MAX_SPRITE_ARRAYS> in_use;
        std::array<sprite_array_t, SPK_MAX_SPRITE_ARRAYS> sprite_arrays;
    };
}
