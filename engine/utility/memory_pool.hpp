#pragma once

#include "base.hpp"
#include "static_list.hpp"

namespace spk {
    template<typename T>
    T* inc_by_byte(T* ptr, size_t byte) {
        return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(ptr) + byte);
    }
    
    enum block_flags_e: uint8_t {
        BLOCK_FLAGS_FREE        = 0,
        BLOCK_FLAGS_INITIALIZED = 1,
        BLOCK_FLAGS_IN_USE      = 1 << 0
    };

    template<typename T>
    struct block_header_t {
        std::bitset<8> flags;

        // will be null if the block is allocated
        block_header_t<T>* prev;
        block_header_t<T>* next;

        // the amount of bytes, including the header and its blocks it occupies
        size_t total_bytes; 

        size_t block_count() const {
            return (total_bytes - sizeof(block_header_t<T>)) / sizeof(T);
        }

        T* get_elements() {
            return inc_by_byte((T*)this, sizeof(*this));
        }
    };

    template<typename T>
    class block_allocator_t {
        static constexpr size_t block_header_size = sizeof(block_header_t<T>);
        static constexpr size_t alignment   = 8; 

    public:
        block_allocator_t(size_t capacity = 10) {
            size_t alignment_offset = 0;

            this->first = nullptr;
            this->last  = nullptr;
            this->capacity = (capacity * sizeof(T) + capacity * block_header_size);

            this->real_ptr = (uint8_t*)malloc(this->capacity + alignment);
            alignment_offset = alignment - ((size_t)real_ptr % alignment);

            if(alignment_offset == alignment) {
                this->aligned_ptr = real_ptr;
            } else {
                this->aligned_ptr = real_ptr + alignment_offset;
            }

            create_block_header(this->aligned_ptr, this->capacity);
        }

        ~block_allocator_t() {
            if(total_blocks != freed_blocks)
                log.log("some blocks were not free'd, %s", typeid(block_allocator_t<T>).name());

            free(real_ptr);
        }

        // size is the amount of elements you want allocated
        T* allocate(size_t size) {
            block_header_t<T>* header = get_block_with(size);

            if(header == nullptr) {
                return nullptr;
            }
 
            bisect_block(header, size);
            set_block_as_allocated(header);

            return header->get_elements();
        }

        void deallocate(T* block) {
            block_header_t<T>* header = (block_header_t<T>*)((uint8_t*)block - block_header_size);

            spk_assert(block != nullptr);
            spk_assert(!header->flags[BLOCK_FLAGS_FREE]);

            header->flags[BLOCK_FLAGS_FREE].flip();

            freed_blocks++;

            free_list_push_back(header);
        }

        size_t get_capacity() const {
            return capacity;
        }

        bool ptr_in_bounds(T* ptr) const {
            return aligned_ptr <= (uint8_t*)ptr && (uint8_t*)ptr <= aligned_ptr + capacity;
        }

    protected:
        block_header_t<T>* get_block_with(size_t size) {
            block_header_t<T>* header = nullptr;
            
            header = find_header_with_at_least(size);
            if(header != nullptr) {
                return header;
            }

            header = try_merge_header_with_at_least(size);
            if(header != nullptr) {
                return header;
            }

            return nullptr;
        }

        block_header_t<T>* find_header_with_at_least(size_t size) {
            size_t total_bytes = 0;

            block_header_t<T>*       header = (block_header_t<T>*)aligned_ptr;
            const block_header_t<T>* end    = (block_header_t<T>*)(aligned_ptr + this->capacity);
            for(; header != end; header = inc_by_byte(header, header->total_bytes)) {
                total_bytes += header->total_bytes;
            }

            log.log("[em] total bytes in all blocks: %llu ? %llu [reset]", total_bytes, capacity);

            for(block_header_t<T>* cur = first; cur != nullptr; cur = cur->prev) {
                if(first->block_count() >= size) {
                    return cur;
                }
            }

            return nullptr;
        }

        block_header_t<T>* try_merge_header_with_at_least(size_t size) {
            // we take advantage of the fact that block headers
            // are right next to each other in memory (with some elements in between)
            // -knowing this allows us to merge multiple headers if they are:
            // 1. free'd and
            // 2. contigous in memory

            size_t             total_contigous_bytes = 0;
            block_header_t<T>* block_begin            = nullptr;
            block_header_t<T>* block_end              = nullptr;

            // there is always a valid header at aligned ptr
            block_header_t<T>*       header = (block_header_t<T>*)aligned_ptr;
            const block_header_t<T>* end    = (block_header_t<T>*)(aligned_ptr + this->capacity);
            for(; header != end; header = inc_by_byte(header, header->total_bytes)) {
                size_t usable_user_memory = 0;

                if(header->flags[BLOCK_FLAGS_FREE]) {
                    total_contigous_bytes += header->total_bytes;

                    if(block_begin == nullptr) {
                        block_begin = header;
                    }
                } else {
                    total_contigous_bytes = 0;
                    block_begin = nullptr;
                    continue;
                }

                usable_user_memory = total_contigous_bytes - block_header_size;
                if(usable_user_memory >= sizeof(T) * size) {
                    block_end = (block_header_t<T>*)((uint8_t*)header + header->total_bytes);
                    break;
                }
            }

            if(block_end == nullptr)
                return nullptr;

            // remove all headers from list
            for(auto cur = block_begin; cur != block_end; cur = inc_by_byte(cur, cur->total_bytes)) {
                free_list_remove(cur);
            }

            return create_block_header((uint8_t*)block_begin, total_contigous_bytes);
        }

        /**
         * @brief bisects a free block into two differently sized blocks
         * 
         * @param header the header of the block to bisect
         * @param size the number of elements to preserve in the block
         * @return the front block
         */
        block_header_t<T>* bisect_block(block_header_t<T>* header, size_t size) {
            const size_t       back_block_new_size     = block_header_size + sizeof(T) * size;         
            int32_t            front_block_total_bytes = 0;
            block_header_t<T>* next       = nullptr;

            if(!header->flags[BLOCK_FLAGS_FREE])
                return nullptr;

            front_block_total_bytes = (int32_t)header->total_bytes - (int32_t)back_block_new_size;
            if(front_block_total_bytes < 0)
                return nullptr;

            header->total_bytes = back_block_new_size;
            next = inc_by_byte(header, header->total_bytes);

            return create_block_header((uint8_t*)next, front_block_total_bytes);
        }

        void set_block_as_allocated(block_header_t<T>* header) {
            spk_assert(header->flags[BLOCK_FLAGS_FREE]);

            // claim the block as allocated
            header->flags[BLOCK_FLAGS_FREE].flip();

            free_list_remove(header);

            freed_blocks--;
        }

        // size is in bytes
        block_header_t<T>* create_block_header(uint8_t* addr, size_t size) {
            spk_assert(((size_t)addr % 2) == 0, "address given was not aligned by 2");
            spk_assert((int32_t)size - block_header_size >= 0);

            block_header_t<T>* header = (block_header_t<T>*)addr;
            header->total_bytes = size;

            header->flags       = 0; 
            header->next        = nullptr;
            header->prev        = nullptr;
    
            header->flags[BLOCK_FLAGS_FREE].flip();

            free_list_push_back(header);

            total_blocks++;
            freed_blocks++;

            return header;
        }

    private:
        size_t capacity; // in bytes

        size_t   total_blocks = 0;
        size_t   freed_blocks = 0;
        uint8_t* real_ptr;
        uint8_t* aligned_ptr;

        // linked list of free blocks
        block_header_t<T>* first;
        block_header_t<T>* last;

    protected:
        // linked list operations
        void free_list_assign_first(block_header_t<T>* iter) {
            spk_assert(first == nullptr && last == nullptr);

            first = iter;
            last  = iter;
        }

        void free_list_remove(block_header_t<T>* iter) {
            if(iter == first) {
                first = iter->prev;
            } 

            if(iter == last) {
                last = iter->next;
            }

            if(iter->next != nullptr) {
                iter->next->prev = iter->prev;
            }

            if(iter->prev != nullptr) {
                iter->prev->next = iter->next;
            }

            iter->prev = nullptr;
            iter->next = nullptr;
        }

        void free_list_push_back(block_header_t<T>* iter) {
            if(first == nullptr)
                free_list_assign_first(iter);
            else {
                last->prev = iter;
                iter->next = last;
                last = iter;
            }
        }

        void free_list_push_front(block_header_t<T>* iter) {
            if(first == nullptr)
                free_list_assign_first(iter);
            else {
                first->next = iter;
                iter->prev = first;
                first = iter;
            }
        }

        void free_list_pop_front() {
            if(first != nullptr) {
                block_header_t<T>* iter_prev = first->prev;

                first = iter_prev;

                if(iter_prev != nullptr) {
                    iter_prev->next = nullptr;
                }
            }
        }

        void free_list_pop_back() {
            if(last != nullptr) {
                block_header_t<T>* iter_next = last->next;

                last = iter_next;

                if(iter_next != nullptr) {
                    iter_next->prev = nullptr;
                }
            }
        }
    };

    template<typename T>
    class _memory_pool_t {
    public:
        _memory_pool_t(size_t initial_size) {
            this->initial_size = initial_size;

            allocators.emplace_back(initial_size);
        }

        T* allocate(size_t size, const void* hint = 0) {
            T* ptr = nullptr;

            for(auto& allocator : allocators) {
                ptr = allocator.allocate(size);
                if(ptr != nullptr) {
                    return ptr;
                }
            }

            initial_size += size;
            allocators.emplace_back(initial_size);

            return allocators.back().allocate(size);
        }   

        void deallocate(T* ptr, size_t n = 0) {
            for(auto& allocator : allocators) {
                if(allocator.ptr_in_bounds(ptr)) {
                    allocator.deallocate(ptr);
                    return;
                }
            }

            spk_assert(!ptr, "cannot deallocate invalid memory");
        }

        T* address(T& x) {
            return &x;
        }

        void construct(T* ptr, const T& tp) {
            *ptr = tp;
        }
    
        template<typename ... params>
        void construct(T* ptr, params&& ... args) {
            new(ptr)T(args...);
        }

        void destroy(T* ptr) {
            ptr->~T();
        }

    private:
        size_t initial_size;
        std::vector<block_allocator_t<T>> allocators;
    };
}

namespace spk {
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
        static constexpr size_t alignment = 8;

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

        size_t get_size() const {
            return size;
        }

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

        size_t size() {
            spk_trace();

            size_t culm_sum = 0;

            for(auto& column : columns) {
                culm_sum += column.get_size();
            }

            return culm_sum;
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