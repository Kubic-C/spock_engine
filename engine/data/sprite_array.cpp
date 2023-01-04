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
        tex_param_clear(GL_TEXTURE_2D_ARRAY);
        tex_param_wrap_repeat(GL_TEXTURE_2D_ARRAY);
    }

    void sprite_array_manager_t::sarm_init() {
    }

    void sprite_array_manager_t::sarm_free() {
        for(uint32_t i = 0; i < SPK_MAX_SPRITE_ARRAYS; i++) {
            if(in_use[i]) {
                sprite_arrays[i].free();
            }
        }
    }


    void sprite_array_manager_t::sprite_array_init(uint32_t index) {
        spk_assert(!sprite_array_is_in_use(index));

        sprite_arrays[index].init();
        sprite_array_in_use(index);
    }

    bool sprite_array_manager_t::sprite_array_is_in_use(uint32_t index) {
        return in_use[index];
    }

    void sprite_array_manager_t::sprite_array_in_use(uint32_t index) {
        if(!in_use[index])
            in_use.set(index, true);
    }
    
    sprite_array_t* sprite_array_manager_t::sprite_array_get(uint32_t index) {
        spk_assert(sprite_array_is_in_use(index));

        return &sprite_arrays[index];
    }

    void sprite_array_manager_t::sprite_array_start(uint32_t index, uint32_t width, uint32_t height, uint32_t sprites) {
        spk_assert(sprite_array_is_in_use(index));

        sprite_arrays[index].storage(width, height, sprites);
    }

    bool sprite_array_manager_t::sprite_array_load(uint32_t index, const char* path, uint32_t sprite_index) {
        spk_assert(sprite_array_is_in_use(index));
        
        return sprite_arrays[index].image(path, sprite_index);
    }

    void sprite_array_manager_t::sprite_array_finish(uint32_t index) {
        spk_assert(sprite_array_is_in_use(index));      

        sprite_arrays[index].tex_params();
    }
}