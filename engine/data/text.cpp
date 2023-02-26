#include "text.hpp"

namespace spk {
      bool font_t::init() {
        if(!texture.init()) {
            return false;
        }

        return true;
    }

    bool font_t::load_ascii_font(FT_Library lib, int f_width, int f_height, const char* file_path) {
        const float x_padding = 3.0f;
        float x = 0; // cursor within this->texture's data, used for writing
        tallest_glyph = 0;
        widest_glyph = 0;
        width = 0;
        height = 0;

        if(FT_New_Face(lib, file_path, 0, &face)) {
            return false;
        }

        FT_Set_Pixel_Sizes(face, f_width, f_height);

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
            character_t* c_data = nullptr;
            float gx, gwidth, gheight; // g for glyph
            
            if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                continue;
            }

            texture.subdata(GL_UNSIGNED_BYTE, x, 0, GL_RED, face->glyph->bitmap.width, 
                face->glyph->bitmap.rows, face->glyph->bitmap.buffer);

            c_data = &char_map[c];

            gx = x / (float)width;
            gwidth = (float)c_data->size.x / (float)width;
            gheight = (float)c_data->size.y / (float)height;

            // ccw indexing, keep that in mind
            c_data->tex_indices[0] = { gx         , gheight };
            c_data->tex_indices[1] = { gx + gwidth, gheight };
            c_data->tex_indices[2] = { gx + gwidth, 0       };
            c_data->tex_indices[3] = { gx         , 0       };

            x += (float)c_data->size.x + x_padding;
        }

        // texture still bound
        glGenerateMipmap(GL_TEXTURE_2D);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // restore previous alignment

        return true;
    }

    void font_t::free() {
        texture.free();
    }

    bool font_manager_t::fm_init() {
        spk_trace();

        if(FT_Init_FreeType(&ft_lib)) {
            return false;
        } else {
            return true;
        }
    }

    font_t* font_manager_t::font_load_ascii(const char* file_path, int f_width, int f_height) {
        font_t* font = font_pool.allocate(1);
        if(!font)
            return nullptr;

        if(!font->init()) {
            goto failure;
        } else {
            if(!font->load_ascii_font(ft_lib, f_width, f_height, file_path)) {
                goto failure;
            } else {
                goto success;
            }
        }

    failure:
        font_pool.deallocate(font);
        return nullptr;
    success:    
        return font;
    }

    font_t* font_manager_t::get_first_font() {
        return &fonts.front();
    }

    void font_manager_t::fm_free() {
        spk_trace();
        
        FT_Done_FreeType(ft_lib);
    }
}