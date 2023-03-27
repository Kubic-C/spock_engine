/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "sprite_array.hpp"
#include "utility/image_loader.hpp"
#include "internal.hpp"

namespace spk {
    sprite_array_t::sprite_array_t() {
        texture_array.init();
    }

    sprite_array_t::~sprite_array_t() {
        texture_array.free();
    }

    void sprite_array_t::storage(uint32_t width, uint32_t height, uint32_t sprites) {
        texture_array.storage(GL_RGBA8, width, height, sprites);
        this->width = width;
        this->height = height;
        this->sprites = sprites;
    }

    bool sprite_array_t::image(const char* path, uint32_t layer) {
        if(layer >= sprites) // access outside of array
            return false;

        image_loader_t il;
        if(!il.load(path, true, STBI_rgb_alpha))
            return false;

        texture_array.subimage(0, 0, layer, width, height, GL_RGBA, GL_UNSIGNED_BYTE, il.pixels);
    
        il.free();

        return true;
    }

    void sprite_array_t::tex_params() {
        texture_array.bind();
        tex_param_nearest(GL_TEXTURE_2D_ARRAY);
        tex_param_wrap_repeat(GL_TEXTURE_2D_ARRAY);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }

    sprite_array_dictionary_t::sprite_array_dictionary_t() {
        spk_trace();    
    }

    sprite_array_dictionary_t::~sprite_array_dictionary_t() {
        spk_trace();
    }

    bool sprite_array_dictionary_t::is_valid(uint32_t id) {
        return sprite_arrays.find(id) != sprite_arrays.end();
    }

    sprite_array_t& sprite_array_dictionary_t::get(uint32_t id) {
        return sprite_arrays[id];
    }

    uint32_t sprite_array_dictionary_t::add() {
        sprite_arrays[++counter];

        return counter;
    }

    hashmap_t<uint32_t, sprite_array_t>& sprite_array_dictionary_t::map() {
        return sprite_arrays;
    }

    sprite_array_dictionary_t& sprite_arrays() {
        return resources().sprite_arrays;
    }

    uint32_t sprite_array_init(uint32_t width, uint32_t height, uint32_t sprites) {
        uint32_t id = sprite_arrays().add();

        sprite_arrays().get(id).storage(width, height, sprites);

        return id;
    }

    bool sprite_array_load(uint32_t id, const char* path, uint32_t index) {
        auto& array   = sprite_arrays().get(id);
        bool  success = array.image(path, index);

        if(!success)
            return false;

        array.tex_params();

        return true;
    }
}