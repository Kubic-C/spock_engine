/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "text.hpp"
#include "internal.hpp"

namespace spk {
    font_t::font_t() {
        character_array.init();
    }

    font_t::~font_t() {
        character_array.free();
    }

    void character_tex_coords(character_t& character, float zcursor, float texture_width, float texture_height) {
        // g is for glyph
        float gwidth  = character.size.x / texture_width;
        float gheight = character.size.y / texture_height;

        character.tex_indices[0] = {0      ,gheight, zcursor};
        character.tex_indices[1] = {gwidth, gheight, zcursor};
        character.tex_indices[2] = {gwidth ,0      , zcursor};
        character.tex_indices[3] = {0      ,0      , zcursor};
    }

    void character_scale_down(character_t& character, float scale) {
        character.size    *= scale;
        character.advance *= scale;
        character.offset  *= scale;
    }

    void character_load(character_t& character, FT_GlyphSlot glyph) {
        character.flags[CHARACTER_FLAG_VALID] = true;
        character.advance[0] = glyph->advance.x >> 6;
        character.advance[1] = glyph->advance.y >> 6;
        character.size       = {glyph->bitmap.width, glyph->bitmap.rows};
        character.offset[0]  = glyph->bitmap_left; 
        character.offset[1]  = character.size.y - glyph->bitmap_top;
    }

    void font_t::prepare_for_load(float render_scale) {
        widest_glyph  = 0;
        tallest_glyph = 0;
        this->render_scale  = render_scale;
    }

    bool font_t::load_ascii_font(FT_Library lib, int font_size, int render_size, const char* file_path) {
        uint32_t       zcursor = 0; // cursor within this->texture's data, used for writing
        FT_Face        face; 

        prepare_for_load((float)render_size / (float)font_size);

        if(!face_load(&face, file_path, font_size)) {
            return false;
        }

        for(uint8_t c = 0; c < UCHAR_MAX; c++) {
            if(FT_Get_Char_Index(face, c) == 0) {
                // undefined face
                char_map[c].flags[CHARACTER_FLAG_VALID] = false;
                continue;
            }

            if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                continue;
            }

            widest_glyph  = std::max(widest_glyph, face->glyph->bitmap.width);
            tallest_glyph = std::max(tallest_glyph, face->glyph->bitmap.rows);

            character_load(char_map[c], face->glyph);
        }


        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        character_array.storage(GL_R8, widest_glyph, tallest_glyph, UCHAR_MAX);
        tex_param_wrap_clamp_to_edge(GL_TEXTURE_2D_ARRAY);
        tex_param_linear(GL_TEXTURE_2D_ARRAY);

        for(uint8_t c = 0; c < UCHAR_MAX; c++) {
            if(FT_Load_Char(face, c, FT_LOAD_RENDER) || !char_map[c].flags[CHARACTER_FLAG_VALID]) {
                continue;
            }

            character_array.subimage(0, 0, zcursor, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

            character_tex_coords(char_map[c], zcursor, widest_glyph, tallest_glyph);
            
            zcursor += 1;

            // texture coord calculations need the real size within the texture
            // so we must scale down all advances, size, and offsets AFTER we calculate
            // texture coords, otherwise we'd get scaled down texture coords
            character_scale_down(char_map[c], render_scale);
        }

        // texture still bound
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // restore previous alignment
        
        FT_Done_Face(face);

        return true;
    }

    uint32_t font_dictionary_t::add() {
        counter += 1;

        fonts[counter];

        return counter;
    }

    void font_dictionary_t::remove(uint32_t id) {
        spk_assert(is_valid(id));

        fonts.erase(id);
    }

    bool font_dictionary_t::is_valid(uint32_t id) {
        return fonts.find(id) != fonts.end();
    }

    font_t& font_dictionary_t::get(uint32_t id) {
        spk_assert(is_valid(id));

        return fonts[id];
    }

    hashmap_t<uint32_t, font_t>& font_dictionary_t::map() {
        return fonts;
    }

    font_dictionary_t& fonts() {
        return internal->resources.fonts;
    }

    bool font_library_init() {
        spk_trace();

        // FT_Init will return non-zero if an error
        if(FT_Init_FreeType(&resources().ft_lib)) {
            return false;
        }

        return true;
    }

    void font_library_free() {
        spk_trace();
        
        FT_Done_FreeType(resources().ft_lib);
    }

    uint32_t font_create(const char* file_path, int font_size, int render_size) {
        uint32_t id   = resources().fonts.add();
        font_t&  font = resources().fonts.get(id);

        if(!font.load_ascii_font(resources().ft_lib, font_size, render_size, file_path)) {
            resources().fonts.remove(id);
            return UINT32_MAX;
        } else {
            return id;
        }
    }

    void text_info_t::calculate(const std::string& string, font_t& font, float scale) {
        width  = 0.0f;
        height = 0.0f;

        for(uint8_t c : string) {
            height = std::max(height, (float)font.char_map[c].size.y * scale);
            width += font.char_map[c].advance.x * scale;
        }
    }
}