#pragma once

#include "text.hpp"

// the loading and managing resources external the program
// E.G. fonts, shaders, textures

namespace spk {
    class resource_manager_tt {
    public:
        bool init();
        void free();

        font_tt* load_ascii_font(const char* file_path, int f_width = 0, int f_height = 32);
        font_tt* get_first_font();
    private:
        font_manager_tt font_manager;
    };
}