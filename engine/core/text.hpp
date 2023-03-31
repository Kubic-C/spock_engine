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

/*

text utilities:
- rendering
- font loading and managing
- ui text

*/

namespace spk {
    struct character_t {
        glm::vec2 tex_indices[4];
        glm::ivec2 size;
        glm::vec2 offset;
        glm::vec2 advance;
    };

    class font_t {
    public:
        font_t(); 
        ~font_t();
        bool load_ascii_font(FT_Library lib, int f_width, int f_height, const char* file_path);

    public:
        std::string name;

        uint32_t widest_glyph;
        uint32_t tallest_glyph;
        uint32_t width, height;
        texture2D_t texture;

        FT_Face face; 

        hashmap_t<u_char, character_t> char_map;
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
     * @param file_path: font to load path from
     * @param f_width: font width 
     * @param f_height: font height
     * @return and ID assigned with the font. will return UINT32_MAX if the font 
     * could not be loaded or some error occurred 
     */
    uint32_t font_create(const char* file_path, int f_width, int f_height = 32);
}