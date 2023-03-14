#pragma once

#include "base.hpp"
#include "static_list.hpp"
#include "allocator.hpp"

#define _SPK_QUICK_MEASURE(scope, name) \
    {float start = spk::time.get_time(); scope; float end = spk::time.get_time(); spk::log.log("time: %f " name, end - start);}

namespace spk {
    template<typename T>
    T* inc_by_byte(T* ptr, size_t byte) {
        return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(ptr) + byte);
    }
    
    enum block_flags_e: uint8_t {
        BLOCK_FLAGS_FREE        = 0,
        BLOCK_FLAGS_INITIALIZED = 1, // not used
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

        size_t block_count_max() const {
            return (total_bytes - sizeof(block_header_t<T>)) / sizeof(T);
        }

        T* elements() {
            return inc_by_byte((T*)this, sizeof(*this));
        }
    };

    template<typename T>
    class block_allocator_t {
        static constexpr size_t block_header_size = sizeof(block_header_t<T>);
        static constexpr size_t alignment   = 16; 

    public:
        block_allocator_t() {}
        ~block_allocator_t() {}

        void init(size_t capacity = 10) {
            size_t alignment_offset = 0;

            this->first = nullptr;
            this->last  = nullptr;
            this->capacity_ = (capacity * sizeof(T) + (block_header_size * capacity) / 10);

            this->real_ptr = (uint8_t*)malloc(this->capacity_ + alignment);
            alignment_offset = alignment - ((size_t)real_ptr % alignment);

            if(alignment_offset == alignment) {
                this->aligned_ptr = real_ptr;
            } else {
                this->aligned_ptr = real_ptr + alignment_offset;
            }

            if(!create_block_header(this->aligned_ptr, this->capacity_)) {
                log.log(LOG_TYPE_ERROR, "could not create allocator");
            }
        }

        block_allocator_t(const block_allocator_t& other) {
            this->aligned_ptr  = other.aligned_ptr;
            this->real_ptr     = other.real_ptr;
            this->capacity_     = other.capacity_;
            this->first        = other.first;
            this->last         = other.last;
            this->total_blocks = other.total_blocks;
            this->freed_blocks = other.freed_blocks;
        }

        void free() {
            if(real_ptr != nullptr) {
                ::free(real_ptr);
                return;
            }

            if(total_blocks != freed_blocks)
                log.log("some blocks were not free'd, %s", typeid(block_allocator_t<T>).name());
        }

        // size is the amount of elements you want allocated
        T* allocate(size_t size) {
            block_header_t<T>* header = get_block_with(size);

            if(header == nullptr) {
                return nullptr;
            }
 
            bisect_block(header, size);
            set_block_as_allocated(header);

            return header->elements();
        }

        void deallocate(T* block) {
            block_header_t<T>* header = (block_header_t<T>*)((uint8_t*)block - block_header_size);

            spk_assert(block != nullptr);
            spk_assert(!header->flags[BLOCK_FLAGS_FREE]);

            header->flags[BLOCK_FLAGS_FREE].flip();

            freed_blocks++;

            free_list_push_back(header);
        }

        size_t capacity() const {
            return capacity_;
        }

        bool ptr_in_bounds(T* ptr) const {
            return aligned_ptr <= (uint8_t*)ptr && (uint8_t*)ptr <= (aligned_ptr + capacity_);
        }

        uint8_t* get_real_ptr() const {
            return real_ptr;
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
            for(block_header_t<T>* cur = first; cur != nullptr; cur = cur->prev) {
                if(cur->block_count_max() >= size) {
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
            const block_header_t<T>* end    = (block_header_t<T>*)(aligned_ptr + this->capacity_);
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
                total_blocks--;
                freed_blocks--;
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
            if(front_block_total_bytes < block_header_size)
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

            if(addr + size > (aligned_ptr + capacity_) || size < block_header_size) {
                log.log("... no");
                return nullptr;
            }

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
        size_t capacity_ = 0; // in bytes

        size_t   total_blocks = 0;
        size_t   freed_blocks = 0;
        uint8_t* real_ptr     = nullptr;
        uint8_t* aligned_ptr  = nullptr;

        // linked list of free blocks
        block_header_t<T>* first = nullptr;
        block_header_t<T>* last  = nullptr;

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
    class memory_pool_t : public allocator_t<T>{
    public:
        template <typename U> 
        struct rebind { typedef memory_pool_t<U> other; };
    public:
        static constexpr size_t _initial_size = 100;

        template<typename U>
        memory_pool_t(const memory_pool_t<U>& other) {
            this->initial_size = other.get_initial_size();
        }

        memory_pool_t() {
            this->initial_size = _initial_size;
        }

        memory_pool_t(size_t initial_size) {
            this->initial_size = initial_size;
        }

        memory_pool_t(const memory_pool_t<T>& other) {
            this->initial_size = other.initial_size;
            this->allocators = other.allocators;
        }

        memory_pool_t<T>* operator=(const memory_pool_t<T>& other) {
            this->initial_size = other.initial_size;
            this->allocators = other.allocators;    

            return this;
        }

        ~memory_pool_t() {
            for(auto& allocator : allocators) {
                allocator.free();
            }
        }

        T* allocate(size_t size, const void* hint = 0) override {
            T* ptr = nullptr;

            for(size_t i = allocators.size() - 1; i != SIZE_MAX; i--) {
                ptr = allocators[i].allocate(size);
                if(ptr != nullptr) {
                    return ptr;
                }
            }

            initial_size += size;
            allocators.emplace_back();
            allocators.back().init(initial_size);

            return allocators.back().allocate(size);
        }   

        void deallocate(T* ptr, size_t n = 0) override {
            for(auto& allocator : allocators) {
                if(allocator.ptr_in_bounds(ptr)) {
                    allocator.deallocate(ptr);
                    return;
                }
            }

            spk_assert(!ptr, "cannot deallocate invalid memory");
        }

        size_t get_initial_size() const {
            return initial_size;
        }

        const std::vector<block_allocator_t<T>>& get_allocators() const {
            return allocators;
        }

    protected:
        size_t initial_size;
        std::vector<block_allocator_t<T>> allocators;
    };

    /* destroys and creates objects upon creating and destroying */
    template<typename T>
    class object_pool_t : protected memory_pool_t<T> {
    public:
        object_pool_t() {
            this->initial_size = this->_initial_size;
        }

        object_pool_t(size_t initial_size) {
            this->initial_size = initial_size;
        }

        template<typename ... params>
        T* create(size_t size, params&& ... args) {
            T* elements = this->allocate(size);
        
            for(size_t i = 0; i < size; i++) {
                this->construct(elements + i, args...);
            }

            return elements;
        }

        void destruct(T* ptr, size_t size) {
            for(size_t i = 0; i < size; i++) {
                this->destroy(ptr + i);
            }

            this->deallocate(ptr, size);
        }
    };

    template<typename T>
    bool operator==(const block_allocator_t<T>& _1, const block_allocator_t<T>& _2) {
        return _1.get_real_ptr() == _2.get_real_ptr();
    }

    template<typename T>
    bool operator!=(const block_allocator_t<T>& _1, const block_allocator_t<T>& _2) {
        return _1.get_real_ptr() != _2.get_real_ptr();
    }

    template<typename T>
    bool operator==(const memory_pool_t<T>& _1, const memory_pool_t<T>& _2) {
        return _1.get_allocators() == _2.get_allocators();
    }

    template<typename T>
    bool operator!=(const memory_pool_t<T>& _1, const memory_pool_t<T>& _2) {
        return _1.get_allocators() != _2.get_allocators();
    }
}