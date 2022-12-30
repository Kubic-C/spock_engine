#pragma once

#include "memory.hpp"

/* data structures that just aren't quite low level enough to be part of 
   the memory file */

namespace spk {
    template<typename T, size_t capacity>
    class static_list {
    public:
        void init();
        void free();
        
        void push_back(const T& obj);
        void erase(size_t start, size_t end);
        void erase(size_t index);
        void pop_back();
        T&   emplace_back();
    
        size_t size() {
            return count;
        }

        T* begin() {
            return data();
        }

        T* end() {
            return data() + count;
        }

        T* data() { 
            return list.data(); 
        }

        T* back() { 
            return &list[count - 1]; 
        }

        T* front() { 
            return &list[0];
        }

        T& operator[](size_t index) {
            return list[index];
        }
    private:
        std::array<T, capacity> list;
        size_t count;
    };

    template<typename T, size_t capacity>
    void static_list<T, capacity>::init() {
        count = 0;
    }

    template<typename T, size_t capacity>
    void static_list<T, capacity>::free() {
    }

    template<typename T, size_t capacity>
    void static_list<T, capacity>::push_back(const T& obj) {
        spk_assert(count < capacity);
        
        count++;
        list[count - 1] = obj;
    }

    template<typename T, size_t capacity>
    T& static_list<T, capacity>::emplace_back() {
        spk_assert(count < capacity);

        new(&list[count - 1])T();
        return *back();
    }

    template<typename T, size_t capacity>
    void static_list<T, capacity>::erase(size_t start, size_t end) {
        spk_assert(start < end);

        count = (count - end) + start;
        
        memcpy(data() + start, data() + count, sizeof(T) * count);
    }


    template<typename T, size_t capacity>
    void static_list<T, capacity>::erase(size_t index) {
        spk_assert(index < count);

        uint32_t amount_after_index = count - (index + 1);

        // shifting all the elements down by 1
        memcpy(data() + index, data() + amount_after_index, sizeof(T) * amount_after_index);
        count--;    
    }

    template<typename T, size_t capacity>
    void static_list<T, capacity>::pop_back() {
        count--;
    }

}