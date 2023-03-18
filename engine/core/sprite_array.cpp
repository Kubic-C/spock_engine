/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "sprite_array.hpp"
#include "utility/image_loader.hpp"

namespace spk {
    void sprite_array_t::init() {
        texture_array.init();
    }

    void sprite_array_t::free() {
        texture_array.free();
    }

    void sprite_array_t::storage(uint32_t width, uint32_t height, uint32_t sprites) {
        texture_array.storage(GL_RGBA8, width, height, sprites);
        this->width = width;
        this->height = height;
        this->sprites = sprites;
    }

    bool sprite_array_t::image(const char* path, uint32_t layer) {
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

    sprite_array_manager_t::sprite_array_manager_t() {
        spk_trace();    
    }

    sprite_array_manager_t::~sprite_array_manager_t() {
        spk_trace();

        for(uint32_t i = 0; i < SPK_MAX_SPRITE_ARRAYS; i++) {
            if(in_use_bits[i]) {
                sprite_arrays[i].free();
            }
        }
    }


    void sprite_array_manager_t::init(uint32_t index) {
        spk_assert(!is_in_use(index));

        sprite_arrays[index].init();
        in_use(index);
    }

    bool sprite_array_manager_t::is_in_use(uint32_t index) {
        return in_use_bits[index];
    }

    void sprite_array_manager_t::in_use(uint32_t index) {
        if(!in_use_bits[index])
            in_use_bits.set(index, true);
    }
    
    sprite_array_t* sprite_array_manager_t::get(uint32_t index) {
        spk_assert(is_in_use(index));

        return &sprite_arrays[index];
    }

    void sprite_array_manager_t::start(uint32_t index, uint32_t width, uint32_t height, uint32_t sprites) {
        spk_assert(is_in_use(index));

        sprite_arrays[index].storage(width, height, sprites);
    }

    bool sprite_array_manager_t::load(uint32_t index, const char* path, uint32_t sprite_index) {
        spk_assert(is_in_use(index));
        
        return sprite_arrays[index].image(path, sprite_index);
    }

    void sprite_array_manager_t::finish(uint32_t index) {
        spk_assert(is_in_use(index));      

        sprite_arrays[index].tex_params();
    }
}