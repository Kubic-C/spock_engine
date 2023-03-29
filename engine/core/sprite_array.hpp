/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "render/opengl.hpp"
#include "utility/structures.hpp"

#define SPK_MAX_SPRITE_ARRAYS 8

namespace spk {
    struct sprite_array_t {
        sprite_array_t();
        ~sprite_array_t();

        void storage(uint32_t width, uint32_t height, uint32_t sprites);
        bool image(const char* path, uint32_t layer);
        void tex_params();

        uint32_t          width;
        uint32_t          height;
        uint32_t          sprites;
        texture_array2D_t texture_array;
    };  

    class sprite_array_dictionary_t {
    public:
        sprite_array_dictionary_t();
        ~sprite_array_dictionary_t();

        uint32_t                             add();
        bool                                 is_valid(uint32_t id);
        sprite_array_t&                      get(uint32_t id);
        hashmap_t<uint32_t, sprite_array_t>& map();

    private:
        hashmap_t<uint32_t, sprite_array_t> sprite_arrays;

        uint32_t counter = 0; // use for creating IDs
    };

    sprite_array_dictionary_t& sprite_arrays();

    /**
     * @brief initializes a new sprite array
     * 
     * @param width: the width of the sprites in sprite array
     * @param height: the height of the sprites in sprite array
     * @param sprites: the number of sprites within the sprite array
     * 
     * @return an ID assigned with the sprite array
     */
    uint32_t sprite_array_create(uint32_t width, uint32_t height, uint32_t sprites);       

    /**
     * @brief gets sprite data from path and sets it into sprite array index
     * 
     * @param id: the ID of the sprite array to change
     * @param path: the path of an image file to load into the sprite array
     * @param index: the index of the sprite within the sprite array to set
     * 
     * @param true for successful path load. 
     */
    bool sprite_array_set(uint32_t id, const char* path, uint32_t index);
}
