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
        sprite_array_manager_t();
        ~sprite_array_manager_t();

        bool            is_in_use(uint32_t index);
        void            in_use(uint32_t index);
        void            init(uint32_t index);
        void            start(uint32_t index, uint32_t width, uint32_t height, uint32_t sprites);
        bool            load(uint32_t index, const char* path, uint32_t sprite_index);
        void            finish(uint32_t index);
        sprite_array_t* get(uint32_t index);

    private:
        std::bitset<SPK_MAX_SPRITE_ARRAYS> in_use_bits;
        std::array<sprite_array_t, SPK_MAX_SPRITE_ARRAYS> sprite_arrays;
    };
}
