#pragma once

#include "framework.hpp"

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
        glm::ivec2 bearing;
        uint32_t advance;
    };

    class font_t {
    public:
        bool init(); 
        bool load_ascii_font(FT_Library lib, int f_width, int f_height, const char* file_path);
        void free();

    public:
        std::string name;

        uint32_t widest_glyph;
        uint32_t tallest_glyph;
        uint32_t width, height;
        sfk::texture2D_t texture;

        FT_Face face; 

        sfk::hashmap_t<character_t, UCHAR_MAX, 2, 0, u_char> char_map;
    };

    class font_manager_t {
    public:
        bool fm_init();
        void fm_free();
        font_t* load_ascii_font(const char* file_path, int f_width = 0, int f_height = 32);
        font_t* get_first_font();

    private:
        sfk::memory_pool_t<font_t, 8, 4> font_pool;
        FT_Library ft_lib; 
    };

    struct text_t {
        std::string str;
        float scalar = 1.0f;
        glm::vec3 color = { 1.0f, 0.0f, 0.0f };

        void set(const char* _s, float scalar, glm::vec3 color, font_t* font = nullptr) {
            str = _s;
            this->scalar = scalar;
            this->color = color;
        }

        void operator=(const char* _s) {
            str = _s;
        }

        void operator=(const std::string& _s) {
            str = _s;
        }

        size_t ssize() { return str.size(); }
    };
}