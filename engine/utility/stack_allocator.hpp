#pragma once

#include "base.hpp"

namespace spk {
    struct stack_block_info_t {
        std::function<void()> deconstruct; // calls the deconstructor
        stack_block_info_t*   prev = nullptr; // the previous block
    };

    class stack_allocator_t {
    public:
        stack_allocator_t(size_t max_size = 4096) {
            memory = (uint8_t*)malloc(max_size);
            last   = nullptr;
            count  = 0;
            this->max_size = max_size;
        }

        ~stack_allocator_t() {
            while(pop());
        }

        template<typename T, typename ... params> 
        T* push(params&& ... args) {
            using block_t =  std::pair<stack_block_info_t, T>;
            const size_t total_size = sizeof(block_t);
            block_t*     block      = nullptr;

            if(max_size <= count) {
                return nullptr;
            }

            block = (block_t*)(memory + count);
            count += total_size;

            // std::function in stack_block_info_t has a constructor
            new(&block->first)stack_block_info_t();

            block->first.prev = last;
            block->first.deconstruct = [=]() {
                block->second.~T();
            };

            last = (stack_block_info_t*)block;

            new(&block->second)T(args...);

            return &block->second;
        }

        // if an element could be popped returns true
        bool pop() {
            if(last == nullptr) {
                return false;
            }

            last->deconstruct();
            last = last->prev;

            return true;
        }

    private:
        uint8_t* memory;
        size_t   count; // the amount bytes in use 
        size_t   max_size;
        stack_block_info_t* last;
    };
}