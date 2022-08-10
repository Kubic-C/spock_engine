#pragma once

#include "framework.hpp"

// the loading and managing resources external the program
// E.G. fonts, shaders, textures

namespace spk {
    struct character_tt {
        glm::vec2 tex_indices[4];
        glm::ivec2 size;
        glm::ivec2 bearing;
        uint32_t advance;

        struct {
            glm::vec2 size; // the size in meters
            glm::vec2 bearing;
            float advance;
        } ppm;
    };

    class font_tt {
    public:
        bool init(); 
        bool load_ascii_font(FT_Library lib, int f_width, int f_height, const char* file_path);
        void free();

    public:
        std::string name;

        uint32_t widest_glyph;
        uint32_t tallest_glyph;
        float one_fourth_tallest_glyph;
        uint32_t width, height;
        sfk::texture2D_tt texture;

        FT_Face face; 

        sfk::hashmap_tt<character_tt, UCHAR_MAX, 2, 0, u_char> char_map;
    };

    class font_manager_tt {
    public:
        bool init();
        bool load_ascii_font(int f_width, int f_height, const char* file_path);
        void free();

        uint32_t last_used_key = 0;
        sfk::hashmap_tt<font_tt, 8, 4, 0, uint32_t> font_map;
        FT_Library ft_lib; 
    };

    class resource_manager_tt {
    public:
        bool init();
        void free();

        bool load_ascii_font(const char* file_path, int f_width = 0, int f_height = 32);
        font_tt* get_font(uint32_t id) { return font_manager.font_map.get_value(id); }
    private:
        font_manager_tt font_manager;
    };
}