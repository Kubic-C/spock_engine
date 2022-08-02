#pragma once

#include "base.hpp"
#include <bitset>

namespace sfk {
    template<typename T, size_t max_size = 256>
    class object_pool_tt {
    public:
        void init(size_t initial_size, size_t realloc_size) {
            blocks.resize(initial_size);
            this->realloc_size = realloc_size;
        }

        void free() {

        }

        T* oalloc_ptr() {
            return nullptr;
        }

        uint32_t get_index(T* ptr) {
            return UINT32_MAX;
        }
        
        void free(T* ptr) {

        }

        uint32_t oalloc_index() {
            return UINT32_MAX;
        }

        T* get_ptr(uint32_t index) {
            return nullptr;
        }

        void free(uint32_t index) {

        }

    private:
        std::bitset<max_size> in_use;
        std::vector<T> blocks;
        size_t realloc_size;
        
        /* e.g.
        ** blocks = [ obj, obj, obj(free), obj(free)]
        ** in_use = [ 1,   1,   0,         0        ]
        **/

    };

    template<uint32_t byte_size>
    struct byte_set { int8_t bytes[byte_size]; };

    template<uint32_t num, typename T>
    struct array { T arr[num]; };
}