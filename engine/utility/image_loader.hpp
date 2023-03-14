#pragma once

#include "base.hpp"

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