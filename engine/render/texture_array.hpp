/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "base.hpp"

namespace spk {
    struct texture_array2D_t {
    public:
        void init();
        void free();

        void bind();
        void storage(uint32_t internal_format, uint32_t width, uint32_t height, uint32_t layer_count);
        void subimage(uint32_t x, uint32_t y, uint32_t z, uint32_t width, uint32_t height, uint32_t texel_format, uint32_t texel_type, void* texels);

        uint32_t id          = UINT32_MAX;
        uint32_t layer_count = 0;
    };
}