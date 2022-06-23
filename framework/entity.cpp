#include "entity.hpp"

// uint64 is made up of 4 16s

namespace sfk {
    uint16_t get_generation_count(uint64_t id) {
        return ((uint16_t*)&id)[2];;    
    }
}