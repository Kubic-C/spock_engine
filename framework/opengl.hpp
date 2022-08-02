#pragma once

#include "base.hpp"
#include "debugger.hpp"

#define BGFX_HANDLE_VALID(handle) (handle.idx != bgfx::kInvalidHandle)

// utility render shit

#define SFK_MAX_VERTEX_LAYOUT_ATTRIBUTES 8

namespace sfk {
    uint32_t create_shader(uint32_t shader_type, const char* file_path);

    struct vertex_attribute_tt {
        size_t index;
        size_t size;
        size_t type;
        bool normalized;
        size_t stride;
        size_t offset;
   
        uint32_t buffer;
    };

    class vertex_layout_tt {
    public:

        vertex_layout_tt& add(size_t index, size_t size, size_t type, bool normalized, 
            size_t stride, size_t offset, uint32_t buffer = UINT32_MAX);
        void set_buffer(uint32_t index, uint32_t buffer);

        /* avoid using the heap on small data */
        vertex_attribute_tt attributes[SFK_MAX_VERTEX_LAYOUT_ATTRIBUTES];
        std::bitset<SFK_MAX_VERTEX_LAYOUT_ATTRIBUTES> attributes_active;
    };

    class vertex_array_tt {
    public:
        void init();
        void free();

        void bind();
        void bind_layout(vertex_layout_tt& layout);

        operator uint32_t() {
            return id;
        }

    private:
        uint32_t id; // OpenGL ID
    };

    class vertex_buffer_tt {
    public:
        void init(uint32_t type);
        void free();
    
        void bind();
        void buffer_data(size_t size, void* data, size_t usage);
        void buffer_sub_data(size_t offset, size_t size, void* data);

        operator uint32_t() {
            return id;
        }

    private:
        uint32_t id; // OpenGL ID
        uint32_t type;
    };

    /* predefined never changing index buffer */
    class static_index_buffer_tt : public vertex_buffer_tt {
    public:
    
        // cap is the maximum amount of quads that the index buffer can 
        // generate for
        void generate_quad_indexes(uint32_t cap);
    };
}