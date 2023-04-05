/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

    refer to license: https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "common.hpp"

namespace spk {
    template<typename T, size_t width, size_t height>
    class array2D_t {
    public:
        T* operator[](size_t x_index) {
            return &data[x_index * width];
        }

        size_t width_get() const { return width; }
        size_t height_get() const { return height; }

    private:
        std::array<T, width * height> data;
    };
}