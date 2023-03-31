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
        texture.init();
    }

    font_t::~font_t() {
        texture.free();
    }

    void character_scale_down(character_t& character, float scale) {
        character.size    *= scale;
        character.advance *= scale;
        character.offset  *= scale;
    }

    bool font_t::load_ascii_font(FT_Library lib, int f_width, int _render_width, const char* file_path) {
        const float x_padding = 3.0f;
        float x = 0; // cursor within this->texture's data, used for writing
        tallest_glyph = 0;
        widest_glyph  = 0;
        width  = 0;
        height = 0;
        render_scale  = (float)_render_width / (float)f_width;

        if(FT_New_Face(lib, file_path, 0, &face)) {
            return false;
        }

        FT_Set_Pixel_Sizes(face, f_width, 0);

        for(u_char c = 0; c < UCHAR_MAX; c++) {
            character_t* ch = nullptr;

            if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                continue;
            }

            width += face->glyph->bitmap.width + x_padding;
            widest_glyph = std::max(widest_glyph, face->glyph->bitmap.width);
            tallest_glyph = std::max(tallest_glyph, face->glyph->bitmap.rows);

            ch = &char_map[c];
            ch->advance[0] = face->glyph->advance.x >> 6;
            ch->advance[1] = face->glyph->advance.y >> 6;
            ch->size       = {face->glyph->bitmap.width, face->glyph->bitmap.rows};

            ch->offset[0] = face->glyph->bitmap_left; 
            ch->offset[1] = ch->size.y - face->glyph->bitmap_top;
        }

        height = tallest_glyph;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        texture.allocate(GL_UNSIGNED_BYTE, GL_RED, GL_RED, width, height, nullptr);
        texture.bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        for(u_char c = 0; c < UCHAR_MAX; c++) {
            character_t* ch = nullptr;
            float gx, gwidth, gheight; // g for glyph
            
            if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                continue;
            }

            texture.subdata(GL_UNSIGNED_BYTE, x, 0, GL_RED, face->glyph->bitmap.width, 
                face->glyph->bitmap.rows, face->glyph->bitmap.buffer);

            ch = &char_map[c];

            gx      = x / (float)width;
            gwidth  = ch->size.x / (float)width;
            gheight = ch->size.y / (float)height;

            // ccw indexing, keep that in mind
            ch->tex_indices[0] = { gx         , gheight };
            ch->tex_indices[1] = { gx + gwidth, gheight };
            ch->tex_indices[2] = { gx + gwidth, 0       };
            ch->tex_indices[3] = { gx         , 0       };

            x += ch->size.x + x_padding;

            character_scale_down(*ch, render_scale);
        }

        // texture still bound
        glGenerateMipmap(GL_TEXTURE_2D);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // restore previous alignment

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

    uint32_t font_create(const char* file_path, int f_width, int render_width) {
        uint32_t id   = resources().fonts.add();
        font_t&  font = resources().fonts.get(id);

        if(!font.load_ascii_font(resources().ft_lib, f_width, render_width, file_path)) {
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