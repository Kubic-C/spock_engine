#include "resource_manager.hpp"

namespace spk {


    bool resource_manager_t::init() {
        if(!font_manager.init())
            return false;

        return true;
    }

    void resource_manager_t::free() {
        font_manager.free();
    }

    font_t* resource_manager_t::load_ascii_font(const char* file_path, int f_width, int f_height) {
        return font_manager.load_ascii_font(f_width, f_height, file_path);
    }

    font_t* resource_manager_t::get_first_font() {
        font_t* font[1];

        font_manager.font_pool.get_valid_blocks(font, 1);

        return font[0];
    }
}