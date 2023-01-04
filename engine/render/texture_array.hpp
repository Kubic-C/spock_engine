#pragma once

#include "base.hpp"

namespace spk {
    class texture_array2D_t {
    public:
        void init();
        void free();

        void bind();
        void storage(uint32_t internal_format, uint32_t width, uint32_t height, uint32_t layer_count);
        void subimage(uint32_t x, uint32_t y, uint32_t z, uint32_t width, uint32_t height, uint32_t texel_format, uint32_t texel_type, void* texels);

    private:
        uint32_t id;
        uint32_t layer_count;
    };
}