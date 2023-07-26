/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

    refer to license: https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "font.hpp"
#include "core/internal.hpp"

namespace spk {
    bool face_load(ptm::ptr_t<FT_Face> face, const char* path, int size) {
        if(FT_New_Face(resources().ft_lib, path, 0, face)) {
            return false;
        }

        FT_Set_Pixel_Sizes(*face, size, 0);

        return true;
    }
}