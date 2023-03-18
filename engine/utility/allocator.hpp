/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

    refer to license: https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once 

#include "memory_pool.hpp"

namespace spk {
    template<typename T>
    class allocator_t {
    public:
      typedef T              _Tp;
      typedef _Tp            value_type;
      typedef size_t         size_type;
      typedef ptrdiff_t      difference_type;
      typedef _Tp*           pointer;
      typedef const _Tp*     const_pointer;
      typedef _Tp&           reference;
      typedef const _Tp&     const_reference;
      typedef allocator_t<T> allocator_type;

      template <class U> 
      struct rebind { typedef allocator_t<U> other; };

    public:
        virtual T* address(T& x) {
            return &x;
        }

        virtual void construct(T* ptr, const T& tp) {
            *ptr = tp;
        }
    
        template<typename ... params>
        void construct(T* ptr, params&& ... args) {
            new(ptr)T(args...);
        }

        virtual void destroy(T* ptr) {
            ptr->~T();
        }

        virtual T* allocate(size_t n, const void* hint = 0) {
            return (T*)calloc(n, sizeof(T));
        }   

        virtual void deallocate(T* ptr, size_t n) {
            free(ptr);
        }

        virtual bool operator==(const allocator_t& alloc) {
            return true;
        }

        virtual bool operator!=(const allocator_t& alloc) {
            return true;
        }
    };

    template<typename T>
    class indirect_allocator_t : public allocator_t<T> {
    public:
        template <typename U> 
        struct rebind { typedef indirect_allocator_t<U> other; };
    public:
        template<typename U>
        indirect_allocator_t(const indirect_allocator_t<U>& other) {
            this->allocator = (allocator_t<T>*)other.get_allocator();
        }

        indirect_allocator_t(const indirect_allocator_t<T>& other) {
            this->allocator = other.allocator;
        }

        indirect_allocator_t(allocator_t<T>* allocator) {
            this->allocator = allocator;
        }

        T* allocate(size_t n, const void* hint = 0) override {
            return allocator->allocate(n, hint);
        }   

        void deallocate(T* ptr, size_t n) override {
            allocator->deallocate(ptr, n);
        }

        allocator_t<T>* get_allocator() const {
            return this->allocator;
        }
    
    private:
        allocator_t<T>* allocator;
    };

    template<typename T>
    bool operator==(const indirect_allocator_t<T>& _1, const indirect_allocator_t<T>& _2) {
        return _1.get_allocator() == _2.get_allocator();
    }

    template<typename T>
    bool operator!=(const indirect_allocator_t<T>& _1, const indirect_allocator_t<T>& _2) {
        return _1.get_allocator() != _2.get_allocator();
    }

}