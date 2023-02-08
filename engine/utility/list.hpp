#pragma once 

#include "memory_pool.hpp"

namespace spk {
    template<typename T>
    struct list_block_t {
        T ele;

        T* prev = nullptr;
        T* next = nullptr;
    };

    template<typename T>
    class list_t {
    public:
        using list_iterator_t = list_block_t<T>;

        list_t() {
            first = nullptr;
            last  = nullptr;
        }

        ~list_t() {
        
        }

        void push_back(const T& ele) {
            list_iterator_t iter = _emplace_back();

            memcpy(&iter.ele, sizeof(T), &ele);
        }

        void push_front(const T& ele) {
            list_iterator_t iter = _emplace_front();

            memcpy(&iter.ele, sizeof(T), &ele);
        }

    protected:
        list_iterator_t allocate() {
            list_iterator_t iter = object_pool.alloc();

            spk_assert(iter != nullptr, "list error: could not allocate from object pool");
       
            return iter;
        }

        void _assign_first(list_iterator_t iter) {
            spk_assert(first == nullptr && last == nullptr);

            first = iter;
            last  = iter;
        }

        list_iterator_t get_iterator(T* ele) {
            return (list_iterator_t)((uint8_t*)ele - offsetof(list_iterator_t, ele)); 
        }

        T& get_object(list_iterator_t iter) {
            return &iter.ele;
        }

        list_iterator_t _emplace_back() {
            list_iterator_t iter = allocate();

            if(first == nullptr)
                _assign_first(iter);
            else {
                last->prev = iter;
                last = iter;
            }

            return iter;
        }

        list_iterator_t _emplace_front() {
            list_iterator_t iter = allocate();

            if(first == nullptr)
                _assign_first(iter);
            else {
                first->next = iter;
                first = iter;
            }
        }

        void _pop_front() {
            if(first != nullptr) {
                list_iterator_t iter_prev = first->prev;

                object_pool.letgo(first);

                first = iter_prev;

                if(iter_prev != nullptr) {
                    iter_prev->next = nullptr;
                }
            }
        }

        void _pop_back() {
            if(last != nullptr) {
                list_iterator_t iter_next = last->next;

                object_pool.letgo(last);

                last = iter_next;

                if(iter_next != nullptr) {
                    iter_next->prev = nullptr;
                }
            }
        }

    protected:

        list_iterator_t* first;
        list_iterator_t* last;

        memory_pool_t<list_block_t<T>> object_pool;
    };
}