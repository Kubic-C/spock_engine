#include "image_loader.hpp"
#include "external/stb_image.h"

namespace spk {
    bool image_loader_t::load(const char* path, bool flip, int wanted_channels) {
        stbi_set_flip_vertically_on_load(flip);
        pixels = stbi_load(path, &width, &height, &channels, wanted_channels);
        if(!pixels) {
            return false;
        } 

        stbi_set_flip_vertically_on_load(!flip);

        return true;
    }

    void image_loader_t::free() {
        stbi_image_free(pixels);
    }
}