#pragma once

#include "base.hpp"

namespace spk {
    template<typename T>
    class array2D_t {
    public:
        void init(uint32_t width, uint32_t height) {
            data         = new T[width * height];
            this->width  = width;
            this->height = height;
        }

        void free() {
            delete data;
        }

        T& get(uint32_t x, uint32_t y) {
            return *get_from_2D(x, y);
        }

        T* get_from_1D(uint32_t i) {
            return data + i;
        }

        T* get_from_2D(uint32_t x, uint32_t y) {
            return data + get_1D_from_2D(x, y);
        }

        uint32_t get_1D_from_2D(uint32_t x, uint32_t y) {
            return (y * width) + x;
        }

        glm::uvec2 get_2D_from_1D(uint32_t index) {
            uint32_t y = index / width;
            uint32_t x = index % width;

            return { x, y };
        }

        uint32_t get_width() const {
            return width;
        }

        uint32_t get_height() const {
            return height;
        }

        glm::uvec2 get_size() const {
            return { width, height };
        }

    private:
        T* data;
        uint32_t width;
        uint32_t height;
    };
}