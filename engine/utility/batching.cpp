#include "batching.hpp"

namespace spk {

}

namespace spk {
    void quad_batch_tt::init(uint32_t max_quads) {
        this->quad_cap = max_quads;
        this->quad_size = 0;
        this->vertex_byte_size = sizeof(float) * 2;

        vertex_buffer_size = 0;
        vertex_buffer.init(GL_ARRAY_BUFFER);
        vertex_buffer.buffer_data((vertex_byte_size * 4) * max_quads, nullptr, GL_DYNAMIC_DRAW);

        index_buffer.init(GL_ELEMENT_ARRAY_BUFFER);
        index_buffer.generate_quad_indexes(max_quads);
    }

    void quad_batch_tt::free() {
        vertex_buffer.free();
        index_buffer.free();
    }

    uint32_t quad_batch_tt::add(float x, float y) {
        set(quad_size, x, y);
        quad_size++;

        return quad_size;
    }

    void quad_batch_tt::set(uint32_t index, float x, float y) {
        float pos[] = {
           -(25.f) + x, -(25.f) + y, // bl
            (25.f) + x, -(25.f) + y, // br
           -(25.f) + x,  (25.f) + y, // tl
            (25.f) + x,  (25.f) + y, // tr
        };

        uint32_t byte_offset = index * (vertex_byte_size * 4); // 4 vertex per quad
        uint32_t byte_size = sizeof(pos);

        if((byte_offset + byte_size) <= (quad_cap * (4 * vertex_byte_size))) {
            vertex_buffer.buffer_sub_data(byte_offset, byte_size, pos);
        } else {
            sfk::logger.add_log(sfk::LOG_TYPE_ERROR, "quad_batch_tt::set byte_offset + byte_size outside range of buffer");
        }
    }

    void quad_batch_tt::remove(uint32_t index) {
        assert(false && "lol think again dipshit");
    }
}