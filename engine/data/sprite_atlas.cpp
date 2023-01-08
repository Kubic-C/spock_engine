#include "sprite_atlas.hpp"

namespace spk {
    bool sprite_atlas_t::init(uint32_t sprites_per_row, uint32_t sprites_per_column) {
        if(!texture.init())
            return false;

        this->sprites_per_row = sprites_per_row; 
        this->sprites_per_column = sprites_per_column;
        width_per_sprite = -1;
        height_per_sprite = -1;
        size = { -1, -1 };

        return true;
    }

    bool sprite_atlas_t::load_from_path(const char* path) {
        if(!texture.load_image(path, 0, true, true)) {
            return false;
        }

        const glm::ivec2 tsize = texture.get_size();

        texture.bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glGenerateMipmap(GL_TEXTURE_2D);

        width_per_sprite = (float)tsize.x / (float)sprites_per_row;
        height_per_sprite = (float)tsize.y / (float)sprites_per_column;
        this->size = { static_cast<uint32_t>(tsize.x), static_cast<uint32_t>(tsize.y) };
        
        return true;
    }

    std::array<glm::vec2, 4> sprite_atlas_t::gen_tex_coords(uint32_t tax, uint32_t tay) {
        spk_assert(tax < sprites_per_row && tay < sprites_per_row, "tax or tay is outside of the valid range for the spritesheet");
        float blx = width_per_sprite * tax;
        float bly = (height_per_sprite * tay) -  height_per_sprite;

        return { // CCW indexing
            (glm::vec2){  blx / size.x                   , (bly + height_per_sprite) / size.y}, //tl
            (glm::vec2){ (blx + width_per_sprite) / size.x, (bly + height_per_sprite) / size.y}, // tr 
            (glm::vec2){ (blx + width_per_sprite) / size.x, bly / size.y }, // br
            (glm::vec2){  blx / size.x                    , bly / size.y } // bl
        };
    }

    void sprite_atlas_t::free() {
        texture.free();
    }

    bool sprite_atlas_manager_t::am_init() {
        return true;
    }

    void sprite_atlas_manager_t::am_free() {
        for(uint32_t i = 0; i < SPK_MAX_ATLAS; i++) {
            if(in_use[i]) {
                sprite_atlases[i].free();
            }
        }
    }

    bool sprite_atlas_manager_t::sprite_atlas_is_in_use(uint32_t index) {
        return in_use[index];        
    }

    void sprite_atlas_manager_t::sprite_atlas_in_use(uint32_t index) {
        if(!in_use[index])
            in_use.set(index, true);
    }

    sprite_atlas_t* sprite_atlas_manager_t::get_atlas(uint32_t index) {
        spk_assert(sprite_atlas_is_in_use(index));

        return &sprite_atlases[index];
    }
}