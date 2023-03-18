/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "texture_array.hpp"
#include "opengl.hpp"

namespace spk {
    void texture_array2D_t::init() {
        glGenTextures(1, &id);
    }

    void texture_array2D_t::free() {
        glDeleteTextures(1, &id);
    }

    void texture_array2D_t::bind() {
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);
    }

    void texture_array2D_t::storage(uint32_t internal_format, uint32_t width, uint32_t height, uint32_t _layer_count) {
        bind();
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internal_format, width, height, _layer_count);
        layer_count = _layer_count;
    }

    void texture_array2D_t::subimage(
        uint32_t x, 
        uint32_t y, 
        uint32_t z, 
        uint32_t width, 
        uint32_t height, 
        uint32_t texel_format, 
        uint32_t texel_type, 
        void* texels) {
            
        bind();
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, x, y, z, width, height, 1, texel_format, texel_type, texels);
    }
}