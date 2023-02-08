#pragma once

#include "static_list.hpp"
#include "array2D.hpp"
#include "memory_pool.hpp"
#include "hashmap.hpp"
#include "allocator.hpp"

namespace spk {
    template<typename T>
    void zero(T* dst) {
        memset(dst, 0, sizeof(T));
    }

    template<typename T>
    T null_object() {
        T value;

        memset(&value, 0, sizeof(T));
        return value;
    }

    template<typename T>
    T probably_an_invalid_object() {
        T value;

        memset(&value, INT_MAX, sizeof(T));
        return value;
    }

    template<typename T>
    bool bytecmp(const T& _1, const T& _2) {
        return memcmp(&_1, &_2, sizeof(T));
    }
}