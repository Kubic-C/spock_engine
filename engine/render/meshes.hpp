#pragma once

#include "opengl.hpp"
#include "utility/structure.hpp"

namespace spk {
    struct comp_mesh_t {
        vertex_buffer_t vertex_buffer;

        void m_init();
        void resize(size_t vertexes);
        void m_free();
    };
}