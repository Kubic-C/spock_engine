/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

    refer to license: https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "ubase.hpp"

namespace spk {
    bool face_load(ptr_t<FT_Face> face, const char* path, int size);
}