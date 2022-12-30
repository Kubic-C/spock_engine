#include "meshes.hpp"

namespace spk {
    void comp_mesh_t::m_init() {
        vertex_buffer.init(GL_ARRAY_BUFFER);
    }
    
    void comp_mesh_t::resize(size_t max_vertexes) {
        spk_assert(!"make this function you fucking dumbass");
    }

    void comp_mesh_t::m_free() {
        vertex_buffer.free();
    }
}