#pragma once

#include "base.hpp"

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

    struct c_cast {};

    template<typename T>
    class ptr_t {
    public:
        template<typename U>
        static constexpr bool castable = std::is_convertible_v<U*, T*> || std::is_convertible_v<T*, U*>;
    
        ptr_t() {}
                                                                 /* allows upcasting vvv */
        template<typename U, bool can_convert = castable<U>>
        ptr_t(ptr_t<U> other) {
            static_assert(can_convert == true);

            object = static_cast<T*>(other.get());
        }

        ptr_t(T* object)
            : object(object) {}

        bool is_null() const {
            return object == nullptr;
        }

        void set(T* new_object) {
            object = new_object;
        }

        T* get() const {
            return object;
        }

        T& operator*() {
            return *object;
        }

        T* operator->() const {
            return object;
        }

        template<typename U, bool can_convert = castable<U>>
        operator U*() {
            return cast<U, can_convert>();
        }

        template<typename U, bool can_convert = castable<U>>
        U* cast() {
            static_assert(can_convert == true);
            
            return static_cast<U*>(object);    
        }

    private:
        T* object; 
    }; 

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
}


template<typename T, typename U>
bool operator==(spk::ptr_t<T> _1, spk::ptr_t<U> _2) {
    return (uint8_t*)_1.get() == (uint8_t*)_2.get();
}

template<typename T, typename U>
bool operator!=(spk::ptr_t<T> _1, spk::ptr_t<U> _2) {
    return (uint8_t*)_1.get() != (uint8_t*)_2.get();
}

template<typename T, typename U>
bool operator==(spk::ptr_t<T> _1, U _2) {
    return (uint8_t*)_1.get() == (uint8_t*)_2;
}

template<typename T, typename U>
bool operator!=(spk::ptr_t<T> _1, U _2) {
    return (uint8_t*)_1.get() != (uint8_t*)_2;
}