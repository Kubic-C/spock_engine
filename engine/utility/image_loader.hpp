/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

    refer to license: https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "ubase.hpp"

namespace spk {
    struct image_loader_t {
        void* pixels;
        int   width;
        int   height;
        int   channels;

        // wanted channels must be STBI_RGB_alpha, STBI_RGB, ectc..
        bool load(const char* path, bool flip = false, int wanted_channels = 0);
        void free();
    };
}