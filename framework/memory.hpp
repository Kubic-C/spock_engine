#pragma once

#include "base.hpp"
#include <bitset>

#ifndef NDEBUG
#define DEBUG_VALUE(type, value) type value
#define DEBUG_EXPR(expr) expr
#else // not debugging
#define DEBUG_VALUE(type, value) 
#define DEBUG_EXPR(expr)
#endif

namespace sfk {
    inline uint32_t str_hash(std::string str) {
        return (str.size() ^ 0xfffffffff);
    }

    template<typename T> 
    inline uint32_t xor_int_hash(T ikey) {
        return ikey ^ UINT32_MAX;
    }

    typedef uint32_t key_t;

    constexpr key_t NULL_KEY = 0U;

    template<class ret, typename ... params>
    using function = ret(*)(params...);

    template<typename T, uint32_t size, size_t alignment, size_t padding = 0, typename keyT = key_t>
    class hashmap_t;

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
    T& make_null_reference() {
        return *(T*)nullptr;
    }

    template<typename T>
    bool bytecmp(const T& _1, const T& _2) {
        return memcmp(&_1, &_2, sizeof(T));
    }

                      // maximum size | alginment (power of two) 
    template<typename T, uint32_t size, size_t alignment, size_t padding = 0>
    class memory_pool_t {
    public:
        using data_callback_t = std::function<bool(T&)>;

        bool init();
        void free();

        T* malloc();
        uint32_t get_valid_blocks(T** ptrs, uint32_t count, data_callback_t clbk = nullptr);
        void letgo(T* ptr);

        uint32_t get_allocated() { return currently_allocated; }

        typedef char block_flags_t;
        enum block_flags_e: char {
            BLOCK_FLAGS_NONE = 0b0,
            BLOCK_FLAGS_FREE = 0b00000001
        };

    private:
        std::array<block_flags_t, size> block_flags;
        uint32_t currently_allocated;
        T* real_ptr;
        T* aligned_ptr;
    };

    /* a high perfomance hashmap for fast use with an attached memory pool .
        a hashmap implementation with in-list collision resolution 
        (no link lists are used to resolve a collision),
        
        avoid using strings as a key type as it will slow the program down
        if the strings are particularly long */
    template<typename T,  uint32_t size, size_t alignment, size_t padding, typename keyT>
    class hashmap_t {
        friend class memory_pool_t<T, size, alignment, padding>;
    public: 

        bool init(::sfk::function<uint32_t, keyT> func);
        void free();

        bool register_key(keyT key);
        T* get_value(keyT key);
        uint32_t get_all_data(T** ptrs, uint32_t count);
        void letgo(keyT key);

        T& operator[](keyT key) {
            return *get_value(key);
        }

    private:
        function<uint32_t, keyT> hash_func;

        const keyT null_obj = null_object<keyT>();

        uint32_t find_matching_index(const keyT& key) {
            uint32_t index = get_index(key);
            uint32_t total_searched = 0;

            for(; total_searched < size; index++, total_searched++) {
                if(index >= size) { // "wrap around"
                    index = 0;
                }
                
                if(translation_table[index].first == key) {
                    break;
                }
            }

            if(total_searched == size) {
                return UINT32_MAX;
            }

            return index;
        }

        inline uint32_t get_index(const keyT& key) {
            uint32_t index = hash_func(key) % size;

            return index;
        }

        std::array<std::pair<keyT, T*>, size> translation_table;
        memory_pool_t<T, size, alignment, padding> pool;
    };

    template<typename T, uint32_t size, size_t alignment, size_t padding>
    bool memory_pool_t<T, size, alignment, padding>::init() {
        size_t offset;
        
        block_flags.fill(BLOCK_FLAGS_FREE);

        // we need to add those few extra bytes of aligment
        // so if there is an offset, we dont sacrifice of the possible max size
        real_ptr = (T*)::malloc(sizeof(T) * (size + padding) + alignment);

        offset = alignment - ((size_t)real_ptr % alignment);
        aligned_ptr = (T*)((char*)real_ptr + offset);

        currently_allocated = 0;

        return true;
    } 

    template<typename T, uint32_t size, size_t alignment, size_t padding>
    void memory_pool_t<T, size, alignment, padding>::free() {   
        ::free(real_ptr);
    }   

    template<typename T, uint32_t size, size_t alignment, size_t padding>
    T* memory_pool_t<T, size, alignment, padding>::malloc() {
        // O(n), let n = size
        T* ptr = NULL;

        for(uint32_t n = 0; n < size; n++) {
            if(block_flags[n] & BLOCK_FLAGS_FREE) {
                // erase the bit on the right (the free bit)
                block_flags[n] &= 0b11111110;

                ptr = aligned_ptr + n;
                break;
            }
        }

        sfk_assert(ptr != null && "memory_pool out of space");
    
        if(ptr) {
            currently_allocated++;
            new(ptr) T();
        }

        return ptr;
    }

    template<typename T, uint32_t size, size_t alignment, size_t padding>
    uint32_t memory_pool_t<T, size, alignment, padding>::get_valid_blocks(T** ptrs, uint32_t count, data_callback_t clbk) {
        uint32_t ptr_it = 0;

        for(uint32_t n = 0; n < size && ptr_it < count; n++) {
            if(!(block_flags[n] & BLOCK_FLAGS_FREE)) {
                if(clbk) {
                    if(clbk(aligned_ptr[n]))
                        return n;
                }

                if(ptrs) {
                    ptrs[ptr_it] = aligned_ptr + n;
                    ptr_it++;
                }
            }
        }

        return ptr_it;
    }

    template<typename T, uint32_t size, size_t alignment, size_t padding>
    void memory_pool_t<T, size, alignment, padding>::letgo(T* ptr) {
        sfk_assert(aligned_ptr <= ptr && (aligned_ptr + size) > ptr);

        /* note to future devs reading this code:
          i HAD to cast everything to size_t when doing pointer arithmetic,
          C++ (im using G++) doesn't like it when I subtract pointers or do division
          it shits its pants and throws the following error:

        !) invalid conversion from ‘long int’ to ‘int*’ [-fpermissive] pointer subtracrion  
            
         if anyone can figure out why this happens that'd be great */

        // 'n' in pointer format 
        size_t n_ptr = (size_t)ptr - (size_t)aligned_ptr;
        // TODO: make this faster
        size_t n = n_ptr / sizeof(T); // division is slow so there could be a better way to do this  
    
        sfk_assert( ! (block_flags[n] & BLOCK_FLAGS_FREE));

        block_flags[n] |= 0b00000001;

        currently_allocated--;

        ptr->~T();
    }

    template<typename T,  uint32_t size, size_t alignment, size_t padding, typename keyT>
    bool hashmap_t<T, size, alignment, padding, keyT>::init(::sfk::function<uint32_t, keyT> hash_func) {
        translation_table.fill(std::pair<keyT, T*>(null_obj, nullptr));

        this->hash_func = hash_func;

        return pool.init();
    }

    template<typename T,  uint32_t size, size_t alignment, size_t padding, typename keyT>
    void hashmap_t<T, size, alignment, padding, keyT>::free() {
        pool.free();
    }

    template<typename T,  uint32_t size, size_t alignment, size_t padding, typename keyT>
    bool hashmap_t<T, size, alignment, padding, keyT>::register_key(keyT key) {
        if(pool.get_allocated() == size) {
            return false;
        } else {            
            uint32_t index = 0;

            index = get_index(key);

            for(;; index++) {
                bool second_is_null;

                if(index >= size) { // "wrap around"
                    index = 0;
                }

                // insure that the key is not a duplicate
                second_is_null = translation_table[index].second == nullptr;
                if(translation_table[index].first == key && !second_is_null)
                    return false;
                
                if(second_is_null) {
                    if((std::get<1>(translation_table[index]) = pool.malloc()) != nullptr)
                        std::get<0>(translation_table[index]) = key;

                    break;
                }
            }
        
            return true;
        }
    }

    template<typename T,  uint32_t size, size_t alignment, size_t padding, typename keyT>
    T* hashmap_t<T, size, alignment, padding, keyT>::get_value(keyT key) {
        uint32_t index = find_matching_index(key);
        
        if(index == UINT32_MAX) {
            return nullptr;
        }

        return translation_table[index].second;
    }

    template<typename T,  uint32_t size, size_t alignment, size_t padding, typename keyT>
    uint32_t hashmap_t<T, size, alignment, padding, keyT>::get_all_data(T** ptrs, uint32_t count) {
        return pool.get_valid_blocks(ptrs, count);
    }

    template<typename T,  uint32_t size, size_t alignment, size_t padding, typename keyT>
    void hashmap_t<T, size, alignment, padding, keyT>::letgo(keyT key) {
        uint32_t index = find_matching_index(key);
        
        translation_table[index].first = null_obj;
        pool.letgo(std::get<1>(translation_table[index]));
        translation_table[index].second = nullptr;
    }
}