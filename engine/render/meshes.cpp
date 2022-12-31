#include "meshes.hpp"

namespace spk {
    void mesh_t::m_init() {
        vertex_buffer.init(GL_ARRAY_BUFFER);
    }
    
    void mesh_t::resize(size_t max_vertexes) {
        spk_assert(!"make this function you fucking dumbass");
    }

    void mesh_t::m_free() {
        vertex_buffer.free();
    }
}