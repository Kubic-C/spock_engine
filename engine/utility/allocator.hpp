#pragma once 

#include "memory_pool.hpp"

namespace spk {
    template<typename T>
    class allocator_t {
    public:
        // for compatibility with the STL
        using   _Tp = T;
        typedef _Tp        value_type;
        typedef size_t     size_type;
        typedef ptrdiff_t  difference_type;
        typedef _Tp*       pointer;
        typedef const _Tp* const_pointer;
        typedef _Tp&       reference;
        typedef const _Tp& const_reference;

    public:
        pointer address(reference x) {
            return &x;
        }

        void construct(T* ptr, const T& tp) {
            *ptr = tp;
        }
    
        template<typename ... params>
        void construct(pointer ptr, params&& ... args) {
            new(ptr)T(args...);
        }

        void destroy(pointer ptr) {
            ptr->~T();
        }

        T* allocate(size_t n, const void* hint = 0) {
            log.log("allocated: %llu", n);

            return (T*)calloc(n, sizeof(T));
        }   

        void deallocate(T* ptr, size_t n) {
            log.log("free'd: %llu", n);
            free(ptr);
        }

    private:
    };
}