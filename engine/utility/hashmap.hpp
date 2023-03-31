/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

    refer to license: https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "ubase.hpp"

namespace spk {
    inline uint32_t str_hash(std::string str) {
        return (str.size() ^ 0xfffffffff);
    }

    template<typename T> 
    inline uint32_t xor_int_hash(T ikey) {
        return ikey ^ UINT32_MAX;
    }

    inline uint32_t xor_u32vec2_hash(glm::u32vec2 vkey) {
        return vkey.x & vkey.y ^ UINT32_MAX;
    }

    typedef uint32_t id_t;

    template<typename KeyT, typename T>
    using hashmap_t = std::unordered_map<KeyT, T>;
}