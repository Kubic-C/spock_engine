#pragma once

#include "text.hpp"
#include "atlas.hpp"
#include "tiles.hpp"

// the loading and managing resources external the program
// E.G. fonts, shaders, textures

namespace spk {
    class resource_manager_t : 
        public font_manager_t, public atlas_manager_t,
        public tile_dictionary_manager_t {
    public:
        bool init();
        void free();
    };
}