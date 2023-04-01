/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "base.hpp"
#include "render/opengl.hpp"
#include "utility/common.hpp"
#include "utility/font.hpp"

namespace spk {
    enum { // goes by the index of bits 
        CHARACTER_FLAG_VALID = 0
    };

    struct character_t {
        std::bitset<8> flags = {0b0};
        glm::vec3 tex_indices[4];
        glm::vec2 size;
        glm::vec2 offset;
        glm::vec2 advance;
    };

    class font_t {
    public:
        font_t(); 
        ~font_t();
        bool load_ascii_font(FT_Library lib, int f_width, int render_width, const char* file_path);

    public:
        std::string name;

        float             render_scale  = 0.0f;
        uint32_t          widest_glyph  = 0.0f;
        uint32_t          tallest_glyph = 0;
        texture_array2D_t character_array;

        hashmap_t<u_char, character_t> char_map;

    private:
        void prepare_for_load(float render_scale);
    };

    class font_dictionary_t {
    public:        
        uint32_t                     add();
        void                         remove(uint32_t id);
        bool                         is_valid(uint32_t id);
        font_t&                      get(uint32_t id);
        hashmap_t<uint32_t, font_t>& map();

    private:
        hashmap_t<uint32_t, font_t> fonts;

        uint32_t counter = 0; // id creation
    };

    struct text_info_t {
        float width;
        float height;

        void calculate(const std::string& string, font_t& font, float scale);
    };

    // gets the font dictionary
    font_dictionary_t& fonts();

    /**
     * @brief initializes the FT_lib object, essentially allows fonts to be 
     * loaded from files. should be called in engine init
     * 
     * @return true if the library could be initialized successfully, false
     * if the library could not be initialized for whatever reason, e.g. 
     * not enough memory.
     */
    bool font_library_init();

    /**
     * @brief cleans up any font resources, should be called in engine free
     * 
     */
    void font_library_free();

    /**
     * @brief creates a new font by loading a font from file_path, f_width and
     * f_height changes the size of a font
     * 
     * @note a higher font_size with a lower render_size will lead to better
     * readability. it is reccomended this be done.
     * 
     * @param file_path: font to load path from
     * @param font_size: the actual font size (in pixels) loaded into memory 
     * @param render_size: the size of the font when rendering
     * @return and ID assigned with the font. will return UINT32_MAX if the font 
     * could not be loaded or some error occurred 
     */
    uint32_t font_create(const char* file_path, int font_size = 128, int render_size = 20);
}