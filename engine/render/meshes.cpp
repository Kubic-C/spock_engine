#include "meshes.hpp"
#include "core/internal.hpp"
#include "render_system.hpp"

namespace spk {
    void mesh_t::m_init() {
        vertex_buffer.init(GL_ARRAY_BUFFER);
        max_vertexes = 0;
    }
    
    void mesh_t::resize(size_t v_size, size_t new_v, void* data) {
        render_system_t* rs           = internal->scene.renderer;
        const size_t     new_buf_size = v_size * new_v;

        vertex_buffer.buffer_data(new_buf_size, data, GL_DYNAMIC_DRAW);
    }

    void mesh_t::m_free() {
        vertex_buffer.free();
    }
}