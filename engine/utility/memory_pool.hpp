#pragma once

#include "base.hpp"
#include "static_list.hpp"

namespace spk {
    constexpr size_t alignment   = 8; 

    enum block_flags_e: uint8_t {
        BLOCK_FLAGS_IN_USE = 1 << 0
    };

    template<typename T>
    struct block_t {
        uint8_t flags = 0;
        T object;

        T* grab() {
            spk_assert(is_free());

            flags |= BLOCK_FLAGS_IN_USE; 
            ctor(&object);
        
            return &object;
        }

        void letgo() {
            spk_assert(is_in_use());

            flags &= ~BLOCK_FLAGS_IN_USE;
            dtor(&object);
        }

        bool is_free() {
            return !(flags & BLOCK_FLAGS_IN_USE);
        }

        bool is_in_use() {
            return flags & BLOCK_FLAGS_IN_USE;
        }
    };

    template<typename T>
    class block_column_t {
    public:
        bool init(size_t capacity) {
            next_alloc = 0;
            size       = 0;
            this->capacity = capacity;

            // this is the aligned block size
            block_size = sizeof(block_t<T>) + (sizeof(block_t<T>) % alignment);

            real_ptr    = (block_t<T>*)calloc(this->capacity, block_size + alignment);
            if(real_ptr == nullptr)
                return false;

            // find the closest aligned boundry in front of real_ptr, and offset by that much
            aligned_ptr = real_ptr + ((size_t)alignment - ((size_t)real_ptr % alignment)); 
        
            return true;
        }

        void free() {
            for(size_t i = 0; i < capacity; i++) {
                if(get(i)->is_in_use()) {
                    get(i)->letgo();
                }
            }
        }

        T* alloc() {
            if(!find_next_alloc())
                return nullptr;
        
            size++;

            return get(next_alloc)->grab();
        }

        bool letgo(T* ptr) {
            block_t<T>* block = nullptr;

            if(!ptr_in_bounds(ptr)) {
                log.log(LOG_TYPE_ERROR, "letgo(): ptr invalid, outside of column's bounds T=(%s), ptr(%p)", typeid(T).name(), ptr);
                return false;
            }

            block = (block_t<T>*)((uint8_t*)ptr - offsetof(block_t<T>, object));
            
            // check and throw a error if the block is already freed
            if(block->is_free()) {
                log.log(LOG_TYPE_ERROR, "letgo(): double free T=(%s), ptr=(%p)", typeid(T).name(), ptr);
                return false;
            }

            size--;
            block->letgo();
            // convert the ptr into an index
            last_free = ((uint8_t*)block - (uint8_t*)aligned_ptr) / block_size;

            return true;
        }

        bool full() {
            return size >= capacity;
        }

        bool ptr_in_bounds(T* ptr) {
            return (uint8_t*)aligned_ptr <= (uint8_t*)ptr && 
                   (uint8_t*)ptr <= (uint8_t*)aligned_ptr + (block_size * capacity);
        }

        size_t find_blocks(T** ptrs, size_t count, const std::function<void(T&)>& clbk) {
            size_t blocks_found = 0;

            // iterate through all blocks
            // if it is valid, i.e. in use:
            // - add it to the list of ptrs, if not null
            // - call the callback

            for(size_t i = 0; i < capacity; i++) {
                if(get(i)->is_in_use()) {
                    blocks_found++;

                    if(blocks_found == count)
                        return blocks_found;

                    if(clbk)
                        clbk(get(i)->object);

                    if(ptrs)
                        ptrs[blocks_found--] = &get(i)->object;
                }
            } 

            return blocks_found;
        }

        size_t get_capacity() const {
            return capacity;
        };

    private:
        block_t<T>* get(uint32_t index) {
            // sizeof(block_t<T>) is not the actual size of blocks allocated in aligned ptr
            // block_size is the actual size
            return (block_t<T>*)((uint8_t*)aligned_ptr + (index * block_size));
        }

        bool swap_next_alloc_if_free(size_t poss_alloc) {
            if(get(poss_alloc)->is_free()) {
                next_alloc = poss_alloc;
                return true;
            }

            return false;
        }

        bool find_next_alloc() {
            // assuming try alloc already points at a block 
            // that is in use, find the next free block 

            if(full()) {
                next_alloc = SIZE_MAX;
                return false;
            }

            // most likely to be a recently free block
            if(swap_next_alloc_if_free(last_free))
                return true;

            // try behind try_alloc
            if(next_alloc != 0) 
                if(swap_next_alloc_if_free(next_alloc - 1)) 
                    return true;

            // try in front of try_alloc
            if(next_alloc != capacity - 1) 
                if(swap_next_alloc_if_free(next_alloc + 1))
                    return true;

            // worst case: iterate through all blocks to find if its free
            for(size_t i = 0; i < capacity; i++) { 
                if(swap_next_alloc_if_free(i))
                    return true;
            }

            // if its not full, it will never get to this point.
            // this is to prevent compiler complaints
            return false;
        }

        size_t      block_size;
        size_t      last_free;
        size_t      next_alloc;
        size_t      size;
        size_t      capacity;
        block_t<T>* real_ptr;
        block_t<T>* aligned_ptr;
    };

    template<typename T, size_t column_max_size = 8>
    class memory_pool_t {
    public:
        memory_pool_t() {
        }

        ~memory_pool_t() {
            for(auto column : columns)
                column.free();
        }

        T* alloc() {
            T* ptr = nullptr;

            for(auto& column : columns) {
                if(column.full())
                    continue;

                return column.alloc();
            }

            // all columns were full
            // a new must be allocated
            if(ptr == nullptr) {
                alloc_column();

                ptr = columns.back()->alloc();
            }

            return ptr;
        }

        bool letgo(T* ptr) {
            spk_assert(columns.get_size() != 0);
            
            for(auto& column : columns) {
                if(column.ptr_in_bounds(ptr)) {
                    column.letgo(ptr);

                    return true;
                }
            }

            return false;
        }
        
        void column_capacity(size_t size) {
            next_capacity = size;
        }

        size_t find_blocks(T** ptrs, size_t count, const std::function<void(T&)>& clbk = nullptr) {
            size_t blocks_found   = 0;

            for(auto& column : columns) {
                if(ptrs)
                    ptrs += blocks_found;

                blocks_found += column.find_blocks(ptrs, count - blocks_found, clbk);

                if(blocks_found >= count) {
                    return blocks_found;
                } 
            }

            return blocks_found;
        }

        size_t capacity() const {
            size_t capacity = 0;

            for(auto& column : columns) {
                capacity += column.get_capacity();
            }

            return capacity;
        }

    private:
        void alloc_column() {
            columns.emplace_back();
            columns.back()->init(next_capacity);
        }

        size_t next_capacity = 100;
        static_list<block_column_t<T>, column_max_size> columns;
    };
}